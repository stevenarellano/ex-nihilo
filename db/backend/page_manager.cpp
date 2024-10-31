#include "page_manager.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "page_manager.h"

PageManager::PageManager(size_t page_size, const std::string& storage_file)
    : page_size_(page_size), next_page_id_(0), storage_file_(storage_file) {
  storage_stream_.open(storage_file,
                       std::ios::in | std::ios::out | std::ios::binary);

  if (!storage_stream_.is_open()) {
    std::ofstream create_file(storage_file, std::ios::out | std::ios::binary);
    if (!create_file) {
      throw std::runtime_error("Unable to create storage file.");
    }
    create_file.close();

    storage_stream_.open(storage_file,
                         std::ios::in | std::ios::out | std::ios::binary);
    if (!storage_stream_.is_open()) {
      throw std::runtime_error("Unable to open storage file after creation.");
    }
  }
}

/**
 * @brief Destructor for the PageManager class.
 */
PageManager::~PageManager() {
  Flush();  // Ensure all dirty pages are flushed to storage
  if (storage_stream_.is_open()) {
    storage_stream_.close();
  }
}

/**
 * @brief Allocates a new page in memory and returns its unique page ID.
 *
 * @return The unique ID of the newly allocated page.
 */
uint32_t PageManager::AllocatePage() {
  uint32_t page_id = next_page_id_++;
  pages_.emplace(page_id,
                 Page{page_id, std::vector<char>(page_size_, 0), false});
  return page_id;
}

/**
 * @brief Retrieves a page by its unique page ID.
 *
 * @param page_id The unique ID of the page to retrieve.
 * @return A pointer to the Page object if successful, or nullptr if an error
 * occurs.
 */
Page* PageManager::GetPage(uint32_t page_id) {
  try {
    auto it = pages_.find(page_id);
    if (it == pages_.end()) {
      Page page = LoadPageFromStorage(page_id);
      it = pages_.emplace(page_id, std::move(page)).first;
    }
    return &it->second;
  } catch (const std::exception& e) {
    return nullptr;
  }
}

void PageManager::MarkDirty(uint32_t page_id) {
  auto it = pages_.find(page_id);
  if (it != pages_.end()) {
    it->second.is_dirty = true;
  } else {
    throw std::runtime_error("Page not found in memory.");
  }
}

void PageManager::Flush() {
  for (auto& [page_id, page] : pages_) {
    if (page.is_dirty) {
      WritePageToStorage(page);
      page.is_dirty = false;
    }
  }
}

void PageManager::ResetPages() {
  pages_.clear();
  next_page_id_ = 0;

  storage_stream_.seekg(0, std::ios::beg);
  storage_stream_.seekp(0, std::ios::beg);
  storage_stream_.clear();
}

void PageManager::FreePage(uint32_t page_id) {
  pages_.erase(page_id);  // Remove from memory
}

Page PageManager::LoadPageFromStorage(uint32_t page_id) {
  Page page;
  page.id = page_id;
  page.is_dirty = false;
  page.data.resize(page_size_);

  std::streampos seek_position = page_id * page_size_;
  storage_stream_.seekg(seek_position, std::ios::beg);

  if (!storage_stream_) {
    storage_stream_.clear();
    throw std::runtime_error("Error seeking to page position in storage.");
  }

  storage_stream_.read(page.data.data(), page_size_);

  if (storage_stream_.eof()) {
    throw std::runtime_error("End of file reached while reading page.");
  } else if (!storage_stream_) {
    throw std::runtime_error("Unknown error occurred while reading page data.");
  }

  return page;
}

void PageManager::WritePageToStorage(const Page& page) {
  if (!storage_stream_.is_open()) {
    throw std::runtime_error("Storage stream is not open.");
  }

  std::streampos seek_position = page.id * page_size_;
  storage_stream_.seekp(0, std::ios::end);
  std::streampos file_size = storage_stream_.tellp();

  if (seek_position >= file_size) {
    storage_stream_.clear();
    storage_stream_.seekp(0, std::ios::end);

    std::vector<char> zero_fill(seek_position - file_size, 0);
    storage_stream_.write(zero_fill.data(), zero_fill.size());

    if (!storage_stream_) {
      storage_stream_.close();
      storage_stream_.open(storage_file_, std::ios::out | std::ios::binary);
      if (storage_stream_) {
        storage_stream_.seekp(0, std::ios::end);
        storage_stream_.write(zero_fill.data(), zero_fill.size());
        storage_stream_.close();
      }

      storage_stream_.open(storage_file_,
                           std::ios::in | std::ios::out | std::ios::binary);
      if (!storage_stream_ ||
          !storage_stream_.seekp(seek_position, std::ios::beg)) {
        throw std::runtime_error(
            "Failed to extend file for page storage after recovery.");
      }
    }
  }

  storage_stream_.seekp(seek_position, std::ios::beg);
  if (!storage_stream_) {
    throw std::runtime_error(
        "Failed to seek to the correct position in storage.");
  }

  storage_stream_.write(page.data.data(), page_size_);
  if (!storage_stream_) {
    throw std::runtime_error("Failed to write page to storage.");
  }
}
