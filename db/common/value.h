#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <stdexcept>

enum class ValueType { INTEGER, DOUBLE };

class Value {
 public:
  ValueType type;
  union {
    int int_val;
    double double_val;
  };

  Value() : type(ValueType::INTEGER), int_val(0) {}
  Value(int val) : type(ValueType::INTEGER), int_val(val) {}
  Value(double val) : type(ValueType::DOUBLE), double_val(val) {}

  ~Value() {}

  bool operator==(const Value& other) const {
    if (type != other.type) {
      return false;
    }
    switch (type) {
      case ValueType::INTEGER:
        return int_val == other.int_val;
      case ValueType::DOUBLE:
        return double_val == other.double_val;
      default:
        throw std::runtime_error("Unknown ValueType in operator==.");
    }
  }

  bool operator!=(const Value& other) const { return !(*this == other); }

  Value operator+(const Value& other) const {
    if (type == ValueType::INTEGER && other.type == ValueType::INTEGER) {
      return Value(int_val + other.int_val);
    } else if (type == ValueType::DOUBLE && other.type == ValueType::DOUBLE) {
      return Value(double_val + other.double_val);
    } else if ((type == ValueType::INTEGER && other.type == ValueType::DOUBLE) ||
               (type == ValueType::DOUBLE && other.type == ValueType::INTEGER)) {
      double lhs = (type == ValueType::DOUBLE) ? double_val : int_val;
      double rhs = (other.type == ValueType::DOUBLE) ? other.double_val : other.int_val;
      return Value(lhs + rhs);  // Mixed integer/double addition
    } else {
      throw std::invalid_argument("Unsupported types for + operation.");
    }
  }

  std::string to_string() const {
    switch (type) {
      case ValueType::INTEGER:
        return "INTEGER";
      case ValueType::DOUBLE:
        return "DOUBLE";
      default:
        throw std::runtime_error("Unknown ValueType.");
    }
  }
};

#endif  // VALUE_H
