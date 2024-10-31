#ifndef TABLE_H
#define TABLE_H

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/value.h"
#include "page_manager.h"
#include "storable.h"

using Row = std::vector<Value>;

struct Column {
 public:
  Column(const std::string& name, ValueType type) : name_(name), type_(type) {}

  const std::string& Name() const { return name_; }
  ValueType Type() const { return type_; }

 private:
  std::string name_;
  ValueType type_;
};

class Table : public Storable {
 public:
  Table(const std::string& name, const std::vector<Column>& columns)
      : Storable(name + ".bin"), name_(name), columns_(columns) {};

  const std::string& Name() const { return name_; };
  const std::vector<Column>& Columns() const { return columns_; }
  std::vector<Row> Data() { return data_; }

  bool Insert(const Row& row);
  // TODO: Finish all DML
  std::vector<Row> Select() { return data_; }

  void Save(PageManager& page_manager) override;
  void Load(PageManager& page_manager) override;

 private:
  std::string name_;
  std::vector<Column> columns_;
  std::vector<Row> data_;

  std::vector<char> Serialize() const override;
  void Deserialize(const std::vector<char>& data) override;
};

#endif  // TABLE_H
