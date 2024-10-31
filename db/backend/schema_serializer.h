#include <cstring>
#include <unordered_map>
#include <vector>

#include "page_manager.h"
#include "table.h"

class SchemaSerializer {
 public:
  template <typename T>
  static void Serialize(const T& object, std::vector<char>& buffer);

  template <typename T>
  static void Deserialize(const std::vector<char>& buffer, size_t& offset,
                          T* object = nullptr);
};

template <typename T>
void SchemaSerializer::Serialize(const T& object, std::vector<char>& buffer) {};
template <typename T>
void SchemaSerializer::Deserialize(const std::vector<char>& buffer,
                                   size_t& offset, T* object) {};

template <>
void SchemaSerializer::Serialize<Column>(const Column& column,
                                         std::vector<char>& buffer) {
  uint32_t name_len = static_cast<uint32_t>(column.Name().size());
  buffer.insert(buffer.end(), reinterpret_cast<char*>(&name_len),
                reinterpret_cast<char*>(&name_len) + sizeof(name_len));
  buffer.insert(buffer.end(), column.Name().begin(), column.Name().end());

  ValueType type = column.Type();
  buffer.insert(buffer.end(), reinterpret_cast<char*>(&type),
                reinterpret_cast<char*>(&type) + sizeof(type));
}

template <>
void SchemaSerializer::Deserialize<Column>(const std::vector<char>& buffer,
                                           size_t& offset, Column* column) {
  uint32_t name_len;
  std::memcpy(&name_len, &buffer[offset], sizeof(name_len));
  offset += sizeof(name_len);
  std::string name(buffer.data() + offset, name_len);
  offset += name_len;
  ValueType type;
  std::memcpy(&type, &buffer[offset], sizeof(type));
  offset += sizeof(type);
  *column = Column(name, type);
}

template <>
void SchemaSerializer::Serialize<Table>(const Table& table,
                                        std::vector<char>& buffer) {
  uint32_t name_len = static_cast<uint32_t>(table.Name().size());
  buffer.insert(buffer.end(), reinterpret_cast<char*>(&name_len),
                reinterpret_cast<char*>(&name_len) + sizeof(name_len));
  buffer.insert(buffer.end(), table.Name().begin(), table.Name().end());

  uint32_t column_count = static_cast<uint32_t>(table.Columns().size());
  buffer.insert(buffer.end(), reinterpret_cast<char*>(&column_count),
                reinterpret_cast<char*>(&column_count) + sizeof(column_count));

  for (const auto& column : table.Columns()) {
    Serialize(column, buffer);
  }
}

template <>
void SchemaSerializer::Deserialize<Table>(const std::vector<char>& buffer,
                                          size_t& offset, Table* table) {
  uint32_t name_len;
  std::memcpy(&name_len, &buffer[offset], sizeof(name_len));
  offset += sizeof(name_len);
  std::string name(buffer.data() + offset, name_len);
  offset += name_len;
  uint32_t column_count;
  std::memcpy(&column_count, &buffer[offset], sizeof(column_count));
  offset += sizeof(column_count);

  std::vector<Column> columns;
  for (uint32_t i = 0; i < column_count; ++i) {
    Column column("", ValueType::INTEGER);  // Default construct
    Deserialize(buffer, offset, &column);
    columns.push_back(column);
  }

  *table = Table(name, columns);
}
