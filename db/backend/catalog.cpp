#include "catalog.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "common/sql_encoding.h"
#include "common/value.h"
#include "page_manager.h"
#include "schema_serializer.h"
#include "table.h"

Catalog::~Catalog() {};

bool Catalog::CreateTable(
    const std::string& table_name,
    const std::vector<ColumnDefinition>& column_definitions) {
  if (tables_.find(table_name) != tables_.end()) {
    return false;
  }

  try {
    std::vector<Column> columns;
    for (const auto& column_definition : column_definitions) {
      columns.push_back(Column(column_definition.name, column_definition.type));
    }

    std::unique_ptr<Table> new_table =
        std::make_unique<Table>(table_name, columns);

    tables_.emplace(table_name, std::move(new_table));

    return true;
  } catch (const std::exception& e) {
    std::cerr << "CreateTable Exception: " << e.what() << "\n";
    return false;
  }
}

bool Catalog::DropTable(const std::string& table_name) {
  auto it = tables_.find(table_name);
  if (it == tables_.end()) {
    return false;
  }

  tables_.erase(it);

  return true;
}

Table* Catalog::GetTable(const std::string& table_name) const {
  auto it = tables_.find(table_name);
  if (it != tables_.end()) {
    return it->second.get();
  }
  return nullptr;
}

const std::string& Catalog::DebugString() {
  static std::string debug_str;
  std::ostringstream oss;

  oss << "Catalog Debug Information:\n";
  oss << "==========================\n";

  for (const auto& [table_name, table_ptr] : tables_) {
    oss << "Table Name: " << table_name << "\n";
    oss << "Columns:\n";

    for (const auto& column : table_ptr->Columns()) {
      oss << "  - Column Name: " << column.Name()
          << ", Type: " << static_cast<int>(column.Type()) << "\n";
    }

    oss << "--------------------------\n";
  }

  debug_str = oss.str();
  return debug_str;
}

void Catalog::Save(PageManager& page_manager) {
  std::vector<char> buffer = Serialize();

  page_manager.ResetPages();
  size_t page_size = page_manager.PageSize();
  size_t offset = 0;

  while (offset < buffer.size()) {
    uint32_t page_id = page_manager.AllocatePage();
    Page* page = page_manager.GetPage(page_id);

    size_t chunk_size = std::min(page_size, buffer.size() - offset);
    page->data.assign(buffer.begin() + offset,
                      buffer.begin() + offset + chunk_size);
    page_manager.MarkDirty(page_id);
    offset += chunk_size;
  }

  page_manager.Flush();
}

void Catalog::Load(PageManager& page_manager) {
  std::vector<char> buffer;
  size_t page_size = page_manager.PageSize();
  uint32_t page_id = 0;

  while (true) {
    Page* page = page_manager.GetPage(page_id);
    if (!page) break;
    page_id++;

    buffer.insert(buffer.end(), page->data.begin(), page->data.end());
  }
  page_manager.SetNextPageId(page_id);
  Deserialize(buffer);
}

std::vector<char> Catalog::Serialize() const {
  std::vector<char> buffer;
  for (const auto& [table_name, table_ptr] : tables_) {
    SchemaSerializer::Serialize(*table_ptr, buffer);
  }
  return buffer;
}

void Catalog::Deserialize(const std::vector<char>& data) {
  size_t offset = 0;
  while (offset < data.size()) {
    Table table("", {});  // Temporary default-constructed table
    SchemaSerializer::Deserialize(data, offset, &table);

    if (table.Name().empty()) {
      break;
    }
    tables_.emplace(table.Name(), std::make_unique<Table>(table));
  }
}