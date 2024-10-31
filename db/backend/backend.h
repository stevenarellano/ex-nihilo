
#include "catalog.h"
#include "common/results.h"
#include "common/sql_encoding.h"
#include "page_manager.h"

static constexpr int kPageSize = 4096;
static constexpr const char* kCatalogFileName = "catalog.db";

class Backend {
 public:
  Backend() : catalog_(std::make_unique<Catalog>(kCatalogFileName)) {
    auto catalog_page_manager =
        std::make_unique<PageManager>(kPageSize, kCatalogFileName);
    page_managers_.emplace(GetCatalog()->FileName(),
                           std::move(catalog_page_manager));
    GetCatalog()->Load(*page_managers_[GetCatalog()->FileName()]);

    std::cout << catalog_->DebugString() << std::endl;
  }

  DDLResult ExecuteDDL(const DDLStatement& ddl_statement);
  std::string ExecuteDML(const DMLStatement& dml_statement);

  Catalog* GetCatalog() { return catalog_.get(); }
  PageManager* FindPageManager(std::string managing_file);

 private:
  std::unique_ptr<Catalog> catalog_;
  std::unordered_map<std::string, std::unique_ptr<PageManager>> page_managers_;
};