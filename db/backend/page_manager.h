#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Page {
  uint32_t id;
  std::vector<char> data;
  bool is_dirty;
};

class PageManager {
 public:
  PageManager(size_t page_size, const std::string& storage_file);
  ~PageManager();

  size_t PageSize() { return page_size_; }
  void SetNextPageId(uint32_t next_page_id) { next_page_id_ = next_page_id; }
  uint32_t GetPageId() { return next_page_id_ - 1; }

  uint32_t AllocatePage();
  Page* GetPage(uint32_t page_id);
  void MarkDirty(uint32_t page_id);
  void Flush();
  void FreePage(uint32_t page_id);
  void ResetPages();

 private:
  size_t page_size_;
  uint32_t next_page_id_;
  std::unordered_map<uint32_t, Page> pages_;
  std::string storage_file_;
  std::fstream storage_stream_;

  Page LoadPageFromStorage(uint32_t page_id);
  void WritePageToStorage(const Page& page);
};

#endif  // PAGE_MANAGER
