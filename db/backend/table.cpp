

#include "table.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

bool Table::Insert(const Row& row) {
  try {
    if (row.size() != columns_.size()) {
      return false;
    }

    for (size_t i = 0; i < row.size(); ++i) {
      if (row[i].type != columns_[i].Type()) {
        std::cerr << "Value type does not match column type for column: "
                  << columns_[i].Name() << "\n";
        return false;
      }
    }

    data_.push_back(row);
    return true;
  } catch (const std::exception& e) {
    std::cerr << "Insert failed: " << e.what() << "\n";
    return false;
  }
}

std::vector<char> Table::Serialize() const {
  std::vector<char> buffer;

  size_t num_rows = data_.size();
  buffer.insert(buffer.end(), reinterpret_cast<const char*>(&num_rows),
                reinterpret_cast<const char*>(&num_rows) + sizeof(size_t));

  for (const Row& row : data_) {
    for (size_t i = 0; i < row.size(); ++i) {
      const Value& value = row[i];
      ValueType type = columns_[i].Type();

      switch (type) {
        case ValueType::INTEGER: {
          int int_val = value.int_val;
          buffer.insert(buffer.end(), reinterpret_cast<const char*>(&int_val),
                        reinterpret_cast<const char*>(&int_val) + sizeof(int));
          break;
        }
        case ValueType::DOUBLE: {
          double double_val = value.double_val;
          buffer.insert(
              buffer.end(), reinterpret_cast<const char*>(&double_val),
              reinterpret_cast<const char*>(&double_val) + sizeof(double));
          break;
        }
        default:
          throw std::runtime_error("Unknown ValueType in serialization.");
      }
    }
  }

  return buffer;
}

void Table::Deserialize(const std::vector<char>& data) {
  data_.clear();
  size_t offset = 0;
  size_t num_rows = 0;

  if (offset + sizeof(size_t) > data.size()) {
    throw std::runtime_error(
        "Invalid data in Deserialize: not enough data for number of rows.");
  }
  std::memcpy(&num_rows, &data[offset], sizeof(size_t));
  offset += sizeof(size_t);

  for (size_t row_idx = 0; row_idx < num_rows; ++row_idx) {
    Row row;
    for (size_t col_idx = 0; col_idx < columns_.size(); ++col_idx) {
      ValueType type = columns_[col_idx].Type();
      switch (type) {
        case ValueType::INTEGER: {
          if (offset + sizeof(int) > data.size()) {
            throw std::runtime_error(
                "Invalid data in Deserialize: not enough data for int value.");
          }
          int int_val = 0;
          std::memcpy(&int_val, &data[offset], sizeof(int));
          offset += sizeof(int);
          row.emplace_back(int_val);
          break;
        }
        case ValueType::DOUBLE: {
          if (offset + sizeof(double) > data.size()) {
            throw std::runtime_error(
                "Invalid data in Deserialize: not enough data for double "
                "value.");
          }
          double double_val = 0;
          std::memcpy(&double_val, &data[offset], sizeof(double));
          offset += sizeof(double);
          row.emplace_back(double_val);
          break;
        }
        default:
          throw std::runtime_error("Unknown ValueType in deserialization.");
      }
    }
    data_.push_back(row);
  }
}

void Table::Save(PageManager& page_manager) {
  std::vector<char> buffer = Serialize();

  size_t page_size = page_manager.PageSize();
  size_t offset = 0;

  page_manager.ResetPages();

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

void Table::Load(PageManager& page_manager) {
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
