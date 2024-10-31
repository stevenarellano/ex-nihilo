#ifndef SQL_ENCODING_H
#define SQL_ENCODING_H

#include <map>
#include <string>
#include <variant>
#include <vector>

#include "value.h"

struct SQLStatement {
  virtual std::string generateSQL() const = 0;
  virtual ~SQLStatement() = default;
};

struct DDLStatement : public SQLStatement {
  virtual ~DDLStatement() = default;
};

struct ColumnDefinition {
  std::string name;
  ValueType type;
};

struct CreateTable : public DDLStatement {
  std::string table_name;
  std::vector<ColumnDefinition> columns;

  std::string generateSQL() const override { return "Unimplemented"; }
};

struct DropTable : public DDLStatement {
  std::string table_name;

  std::string generateSQL() const override {
    return "DROP TABLE " + table_name + ";";
  }
};

struct DMLStatement : public SQLStatement {
  virtual ~DMLStatement() = default;
};

struct Condition {
  std::string column;
  std::string op;  // e.g., '=', '<', '>', 'LIKE', etc.
  Value value;

  Condition(const std::string& col, const std::string& oper, const Value& val)
      : column(col), op(oper), value(val) {}
};

struct Insert : public DMLStatement {
  std::string table_name;
  std::vector<std::string> columns;
  std::vector<std::vector<Value>> values;

  Insert(const std::string& table, const std::vector<std::string>& cols,
         const std::vector<std::vector<Value>>& vals)
      : table_name(table), columns(cols), values(vals) {}

  std::string generateSQL() const override { return "Unimplemented"; }
};

struct Delete : public DMLStatement {
  std::string table_name;
  std::vector<Condition> conditions;

  Delete(const std::string& table, const std::vector<Condition>& conds = {})
      : table_name(table), conditions(conds) {}

  std::string generateSQL() const override { return "Unimplemented"; }
};

struct Update : public DMLStatement {
  std::string table_name;
  std::map<std::string, Value> assignments;
  std::vector<Condition> conditions;

  Update(const std::string& table, const std::map<std::string, Value>& assigns,
         const std::vector<Condition>& conds = {})
      : table_name(table), assignments(assigns), conditions(conds) {}

  std::string generateSQL() const override { return "Unimplemented"; }
};

struct Select : public DMLStatement {
  std::vector<std::string> columns;  // Empty vector means all columns (*)
  std::string table_name;
  std::vector<Condition> conditions;
  bool distinct = false;

  Select(const std::string& table, const std::vector<std::string>& cols = {},
         const std::vector<Condition>& conds = {}, bool dist = false)
      : table_name(table), columns(cols), conditions(conds), distinct(dist) {}

  std::string generateSQL() const override { return "Unimplemented"; }
};

#endif  // SQL_ENCODING_H