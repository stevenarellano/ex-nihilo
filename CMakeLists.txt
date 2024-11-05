cmake_minimum_required(VERSION 3.10)
project(ExNihilo)

enable_testing()

# Include packages
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

option(BUILD_DB "Build db" ON)
if(BUILD_DB)
    add_subdirectory(db)
endif()

option(BUILD_CEREBRUM "Build cerebrum" ON)
if(BUILD_CEREBRUM)
  add_subdirectory(cerebrum) # comes from the subdir CMakeLists.txt
endif()