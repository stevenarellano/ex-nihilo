#ifndef RESULTS_H
#define RESULTS_H

#include <map>
#include <string>
#include <vector>

struct DMLResult {
  bool success;
  int affectedRows;
  std::vector<std::map<std::string, std::string>> data;
  std::string message;

  DMLResult(bool success = false, int affectedRows = 0,
            std::string message = "")
      : success(success),
        affectedRows(affectedRows),
        message(std::move(message)) {}

  DMLResult(const std::vector<std::map<std::string, std::string>>& data)
      : success(true),
        affectedRows(static_cast<int>(data.size())),
        data(data) {}
};

enum class DDLOperationType {
  CREATE,
  ALTER,
  DROP,
};

struct DDLResult {
  bool success;
  std::string message;
  std::string objectName;
  DDLOperationType operation;

  DDLResult(DDLOperationType operation, const std::string& objectName,
            bool success = true, std::string message = "")
      : success(success),
        message(std::move(message)),
        objectName(objectName),
        operation(operation) {}

  DDLResult(DDLOperationType operation, const std::string& objectName,
            std::string message)
      : success(false),
        message(std::move(message)),
        objectName(objectName),
        operation(operation) {}
};

#endif  // RESULTS_H