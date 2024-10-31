#include "backend/catalog.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "backend/page_manager.h"
#include "backend/table.h"
#include "common/sql_encoding.h"
#include "common/value.h"

TEST(CatalogTest, SaveLoad) {
  std::string storage_file = "catalog.bin";
  size_t page_size = 4096;

  std::remove(storage_file.c_str());

  {
    PageManager page_manager(page_size, storage_file);
    Catalog catalog(storage_file);

    std::vector<ColumnDefinition> columns_table1 = {
        {"id", ValueType::INTEGER}, {"age", ValueType::INTEGER}};

    std::vector<ColumnDefinition> columns_table2 = {
        {"product_id", ValueType::INTEGER}};

    bool created_table1 = catalog.CreateTable("Users", columns_table1);
    bool created_table2 = catalog.CreateTable("Products", columns_table2);
    ASSERT_TRUE(created_table1);
    ASSERT_TRUE(created_table2);

    catalog.Save(page_manager);
  }

  {
    PageManager page_manager(page_size, storage_file);
    Catalog catalog(storage_file);

    catalog.Load(page_manager);
    ASSERT_EQ(page_manager.GetPageId(), 0);

    Table* users_table = catalog.GetTable("Users");
    Table* products_table = catalog.GetTable("Products");
    ASSERT_NE(users_table, nullptr);
    ASSERT_NE(products_table, nullptr);

    const auto& users_columns = users_table->Columns();
    ASSERT_EQ(users_columns.size(), 2u);
    ASSERT_EQ(users_columns[0].Name(), "id");
    ASSERT_EQ(users_columns[0].Type(), ValueType::INTEGER);
    ASSERT_EQ(users_columns[1].Name(), "age");
    ASSERT_EQ(users_columns[1].Type(), ValueType::INTEGER);

    const auto& products_columns = products_table->Columns();
    ASSERT_EQ(products_columns.size(), 1u);
    ASSERT_EQ(products_columns[0].Name(), "product_id");
    ASSERT_EQ(products_columns[0].Type(), ValueType::INTEGER);
  }

  {
    PageManager page_manager(page_size, storage_file);
    Catalog catalog(storage_file);

    catalog.Load(page_manager);

    bool dropped_table = catalog.DropTable("Users");
    ASSERT_TRUE(dropped_table);
    Table* users_table = catalog.GetTable("Users");
    ASSERT_EQ(users_table, nullptr);

    catalog.Save(page_manager);
  }

  {
    PageManager page_manager(page_size, storage_file);
    Catalog catalog(storage_file);

    catalog.Load(page_manager);
    ASSERT_EQ(page_manager.GetPageId(), 0);

    Table* users_table = catalog.GetTable("Users");
    Table* products_table = catalog.GetTable("Products");
    ASSERT_EQ(users_table, nullptr);
    ASSERT_NE(products_table, nullptr);

    const auto& products_columns = products_table->Columns();
    ASSERT_EQ(products_columns.size(), 1u);
    ASSERT_EQ(products_columns[0].Name(), "product_id");
    ASSERT_EQ(products_columns[0].Type(), ValueType::INTEGER);
  }
}
