#include <iostream>
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename KeyType, typename ValueType>
class HashTable {
 public:
  HashTable(size_t initial_size = 101) : table_(initial_size) {}

  void insert(const KeyType& key, const ValueType& value) {
    size_t index = hash(key) % table_.size();
    for (auto& pair : table_[index]) {
      if (pair.first == key) {
        pair.second = value;
        return;
      }
    }
    table_[index].emplace_back(key, value);
  }

  ValueType* find(const KeyType& key) {
    size_t index = hash(key) % table_.size();
    for (auto& pair : table_[index]) {
      if (pair.first == key) {
        return &pair.second;
      }
    }
    return nullptr;
  }

  ValueType get(const KeyType& key) {
    ValueType* value = find(key);
    if (!value) {
      throw std::out_of_range("Key not found");
    }
    return *value;
  }

 private:
  std::vector<std::list<std::pair<KeyType, ValueType>>> table_;

  size_t hash(const KeyType& key) const {
    std::hash<KeyType> hasher;
    return hasher(key);
  }
};
