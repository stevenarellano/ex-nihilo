#ifndef CATALOG_H
#define CATALOG_H

#include <memory>
#include <string>
#include <unordered_map>

#include "common/sql_encoding.h"
#include "page_manager.h"
#include "storable.h"
#include "table.h"

class Catalog : public Storable {
 public:
  Catalog(std::string file_name) : Storable(file_name) {};
  ~Catalog();

  bool CreateTable(const std::string& table_name,
                   const std::vector<ColumnDefinition>& column_definitions);
  bool DropTable(const std::string& table_name);

  Table* GetTable(const std::string& table_name) const;

  const std::string& DebugString();

  void Save(PageManager& page_manager) override;
  void Load(PageManager& page_manager) override;

 private:
  std::unordered_map<std::string, std::unique_ptr<Table>> tables_;

  std::vector<char> Serialize() const override;
  void Deserialize(const std::vector<char>& data) override;
};

#endif  // CATALOG_H
