#include "backend/table.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "backend/catalog.h"
#include "backend/page_manager.h"
#include "common/sql_encoding.h"
#include "common/value.h"

TEST(TableTest, SaveLoad) {
  std::string table_name = "employees";
  std::string file_name = table_name + ".bin";
  size_t page_size = 4096;

  std::remove(file_name.c_str());

  std::vector<Column> columns = {Column("id", ValueType::INTEGER),
                                 Column("salary", ValueType::DOUBLE)};

  {
    Table employees(table_name, columns);
    Row valid_row = {Value(1), Value(50000.0)};
    bool insert_success = employees.Insert(valid_row);
    ASSERT_TRUE(insert_success);

    PageManager page_manager(page_size, file_name);
    employees.Save(page_manager);
  }

  {
    PageManager page_manager(page_size, file_name);
    Table employees(table_name, columns);

    employees.Load(page_manager);
    const std::vector<Row>& data = employees.Data();
    ASSERT_EQ(data.size(), 1);

    const Row& loaded_row = data[0];
    ASSERT_EQ(loaded_row.size(), 2);
    EXPECT_EQ(loaded_row[0], Value(1));
    EXPECT_EQ(loaded_row[1], Value(50000.0));
  }
}

TEST(TableTest, InsertInvalidRow) {
  std::vector<Column> columns = {Column("id", ValueType::INTEGER),
                                 Column("salary", ValueType::DOUBLE)};

  Table employees("employees", columns);
  Row invalid_row = {Value(1.0), Value(1.0)};

  bool insert_success = employees.Insert(invalid_row);
  ASSERT_FALSE(insert_success);
}
