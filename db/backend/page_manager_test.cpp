#include "backend/page_manager.h"

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
#include "backend/table.h"
#include "common/sql_encoding.h"
#include "common/value.h"

TEST(PageManagerTest, AllocateWriteFlushReload) {
  std::string storage_file = "test_storage_1.bin";
  size_t page_size = 1024;

  std::remove(storage_file.c_str());

  {
    PageManager page_manager(page_size, storage_file);

    uint32_t page_id = page_manager.AllocatePage();

    Page* page = page_manager.GetPage(page_id);
    ASSERT_NE(page, nullptr);

    const char* test_data = "Hello, World!";
    std::memcpy(page->data.data(), test_data, std::strlen(test_data) + 1);

    page_manager.MarkDirty(page_id);

    page_manager.Flush();
  }

  {
    PageManager page_manager(page_size, storage_file);

    Page* page = page_manager.GetPage(0);
    ASSERT_NE(page, nullptr);

    const char* loaded_data = page->data.data();
    ASSERT_STREQ(loaded_data, "Hello, World!");
  }

  std::remove(storage_file.c_str());
}

TEST(PageManagerTest, DeleteItemsAndReload) {
  std::string storage_file = "test_storage_2.bin";
  size_t page_size = 1024;

  std::remove(storage_file.c_str());

  {
    PageManager page_manager(page_size, storage_file);

    uint32_t page_id = page_manager.AllocatePage();
    Page* page = page_manager.GetPage(page_id);
    ASSERT_NE(page, nullptr);

    int* int_data = reinterpret_cast<int*>(page->data.data());
    size_t num_ints = page_size / sizeof(int);
    for (size_t i = 0; i < num_ints; ++i) {
      int_data[i] = static_cast<int>(i);
    }

    page_manager.MarkDirty(page_id);
    page_manager.Flush();

    for (size_t i = 0; i < num_ints; i += 2) {
      int_data[i] = 0;
    }

    page_manager.MarkDirty(page_id);
    page_manager.Flush();
  }

  {
    PageManager page_manager(page_size, storage_file);

    Page* page = page_manager.GetPage(0);
    ASSERT_NE(page, nullptr);

    int* int_data = reinterpret_cast<int*>(page->data.data());
    size_t num_ints = page_size / sizeof(int);
    for (size_t i = 0; i < num_ints; ++i) {
      if (i % 2 == 0) {
        ASSERT_EQ(int_data[i], 0);
      } else {
        ASSERT_EQ(int_data[i], static_cast<int>(i));
      }
    }
  }

  std::remove(storage_file.c_str());
}

TEST(PageManagerTest, LoadPageFromStorage) {
  std::string storage_file = "test_storage_3.bin";
  size_t page_size = 1024;

  std::remove(storage_file.c_str());

  {
    PageManager page_manager(page_size, storage_file);

    for (uint32_t i = 0; i < 5; ++i) {
      uint32_t page_id = page_manager.AllocatePage();
      Page* page = page_manager.GetPage(page_id);
      ASSERT_NE(page, nullptr);

      std::string data = "Page " + std::to_string(page_id);
      std::memcpy(page->data.data(), data.c_str(), data.size() + 1);

      page_manager.MarkDirty(page_id);
    }

    page_manager.Flush();
  }

  {
    PageManager page_manager(page_size, storage_file);

    uint32_t test_page_id = 2;
    Page* page = page_manager.GetPage(test_page_id);
    ASSERT_NE(page, nullptr);

    const char* data = page->data.data();
    std::string expected = "Page 2";
    ASSERT_STREQ(data, expected.c_str());
  }

  std::remove(storage_file.c_str());
}

TEST(PageManagerTest, FreePage) {
  std::string storage_file = "test_storage_4.bin";
  size_t page_size = 1024;

  std::remove(storage_file.c_str());

  {
    PageManager page_manager(page_size, storage_file);

    uint32_t page_id = page_manager.AllocatePage();
    Page* page = page_manager.GetPage(page_id);
    ASSERT_NE(page, nullptr);

    const char* test_data = "Data before free";
    std::memcpy(page->data.data(), test_data, std::strlen(test_data) + 1);

    page_manager.MarkDirty(page_id);
    page_manager.Flush();

    page_manager.FreePage(page_id);

    try {
      page = page_manager.GetPage(page_id);
      ASSERT_NE(page, nullptr);

      const char* data = page->data.data();
      ASSERT_STREQ(data, test_data);
    } catch (const std::exception& e) {
      std::cerr << "Unexpected error: " << e.what() << std::endl;
      FAIL() << "Exception thrown during test";
    }
  }

  std::remove(storage_file.c_str());
}

TEST(PageManagerTest, StressRandomAccess) {
  std::string storage_file = "test_storage_5.bin";
  size_t page_size = 1024;
  const uint32_t num_pages = 100;

  std::remove(storage_file.c_str());

  std::vector<uint32_t> page_ids;
  std::unordered_map<uint32_t, std::string> page_contents;

  {
    PageManager page_manager(page_size, storage_file);

    for (uint32_t i = 0; i < num_pages; ++i) {
      uint32_t page_id = page_manager.AllocatePage();
      Page* page = page_manager.GetPage(page_id);
      ASSERT_NE(page, nullptr);

      std::string data =
          "Page_" + std::to_string(page_id) + "_Data_" + std::to_string(rand());
      std::memcpy(page->data.data(), data.c_str(), data.size() + 1);

      page_manager.MarkDirty(page_id);

      page_ids.push_back(page_id);
      page_contents[page_id] = data;
    }

    page_manager.Flush();
  }

  {
    PageManager page_manager(page_size, storage_file);

    std::shuffle(page_ids.begin(), page_ids.end(),
                 std::default_random_engine());

    for (uint32_t page_id : page_ids) {
      Page* page = page_manager.GetPage(page_id);
      ASSERT_NE(page, nullptr);

      const char* data = page->data.data();
      ASSERT_EQ(page_contents[page_id], data);
    }
  }

  std::remove(storage_file.c_str());
}
