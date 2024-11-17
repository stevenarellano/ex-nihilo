#include "hash_table.h"

#include <gtest/gtest.h>

class HashTableTest : public ::testing::Test {
 protected:
  void SetUp() override { table = new HashTable<std::string, int>(); }

  void TearDown() override { delete table; }

  HashTable<std::string, int>* table;
};

TEST_F(HashTableTest, InsertAndFindTest) {
  table->insert("key1", 10);
  table->insert("key2", 20);

  auto value1 = table->find("key1");
  ASSERT_NE(value1, nullptr);
  EXPECT_EQ(*value1, 10);

  auto value2 = table->find("key2");
  ASSERT_NE(value2, nullptr);
  EXPECT_EQ(*value2, 20);

  EXPECT_EQ(table->find("key3"), nullptr);
}

TEST_F(HashTableTest, GetTest) {
  table->insert("key1", 30);
  table->insert("key2", 40);

  EXPECT_EQ(table->get("key1"), 30);
  EXPECT_EQ(table->get("key2"), 40);

  EXPECT_THROW(table->get("key3"), std::out_of_range);
}

TEST_F(HashTableTest, UpdateValueTest) {
  table->insert("key1", 50);
  table->insert("key1", 60);

  auto value = table->find("key1");
  ASSERT_NE(value, nullptr);
  EXPECT_EQ(*value, 60);
}

TEST_F(HashTableTest, CollisionHandlingTest) {
  table->insert("key1", 70);
  table->insert("key2", 80);

  auto value1 = table->find("key1");
  ASSERT_NE(value1, nullptr);
  EXPECT_EQ(*value1, 70);

  auto value2 = table->find("key2");
  ASSERT_NE(value2, nullptr);
  EXPECT_EQ(*value2, 80);
}

TEST_F(HashTableTest, StressTest) {
  for (int i = 0; i < 1000; ++i) {
    table->insert("key" + std::to_string(i), i);
  }

  for (int i = 0; i < 1000; ++i) {
    auto value = table->find("key" + std::to_string(i));
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, i);
  }
}
