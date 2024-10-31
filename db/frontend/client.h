#include <string>

#include "backend/backend.h"

class Client {
 public:
  Client(Backend* backend) : backend_(backend) {};

  void ExecuteSql(std::string);

 private:
  Backend* backend_;
};