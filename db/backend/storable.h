#ifndef STORABLE_H
#define STORABLE_H

#include <memory>
#include <string>

#include "page_manager.h"

class Storable {
 public:
  explicit Storable(const std::string& file_name) : file_name_(file_name) {}
  virtual ~Storable() = default;

  virtual void Save(PageManager& page_manager) = 0;
  virtual void Load(PageManager& page_manager) = 0;
  virtual std::string FileName() const { return file_name_; }

 protected:
  std::string file_name_;

  virtual std::vector<char> Serialize() const = 0;
  virtual void Deserialize(const std::vector<char>& data) = 0;
};

#endif  // STORABLE_H
