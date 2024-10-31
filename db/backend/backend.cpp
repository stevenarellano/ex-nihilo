#include "backend.h"

#include <iostream>

#include "common/results.h"
#include "table.h"

DDLResult Backend::ExecuteDDL(const DDLStatement& ddl_statement) {
  bool success = false;
  DDLOperationType ddl_operation = DDLOperationType::CREATE;
  std::string obj_name = "";
  if (auto create_table = dynamic_cast<const CreateTable*>(&ddl_statement)) {
    success = GetCatalog()->CreateTable(create_table->table_name,
                                        create_table->columns);
    ddl_operation = DDLOperationType::CREATE;
    obj_name = create_table->table_name;
  } else if (auto drop_table = dynamic_cast<const DropTable*>(&ddl_statement)) {
    success = GetCatalog()->DropTable(drop_table->table_name);
    ddl_operation = DDLOperationType::CREATE;
    obj_name = drop_table->table_name;
  }

  PageManager* catalog_page_manager = FindPageManager(GetCatalog()->FileName());
  if (catalog_page_manager == nullptr) {
    success = false;
  }
  if (success == true) {
    GetCatalog()->Save(*catalog_page_manager);
  } else {
    std::cout << "Success == False" << std::endl;
  }
  return DDLResult(ddl_operation, obj_name, success);
}

std::string Backend::ExecuteDML(const DMLStatement& dml_statement) {
  if (auto insert = dynamic_cast<const Insert*>(&dml_statement)) {
    auto table = GetCatalog()->GetTable(insert->table_name);
    if (table == nullptr) {
      return "failed";
    }

    bool success = true;
    for (const auto& row : insert->values) {
      success |= table->Insert(row);
      if (!success) {
        break;
      }
    }

    return success ? "success" : "failure";
  }

  return "";
}

PageManager* Backend::FindPageManager(std::string managing_file) {
  auto it = page_managers_.find(managing_file);

  if (it != page_managers_.end()) {
    return it->second.get();
  }
  return nullptr;
}