cmake_minimum_required(VERSION 3.10)
project(db)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



file(GLOB_RECURSE source_files_in_dir
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

foreach(src_file ${source_files_in_dir})
    if(NOT src_file MATCHES "_test\\.cpp$" AND NOT src_file MATCHES "run\\.cpp$")
        list(APPEND db_source_files ${src_file})
    endif()
endforeach()

add_library(database STATIC ${db_source_files})
target_include_directories(database PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(db_app "${CMAKE_CURRENT_SOURCE_DIR}/run.cpp")

target_link_libraries(db_app PRIVATE database)
target_include_directories(db_app PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp")
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})

    target_include_directories(${test_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(${test_name} PRIVATE database GTest::gtest GTest::gtest_main)

    # below is redundant with the addition of gtests
    # add_test(NAME ${test_name} COMMAND ${test_name})

    include(GoogleTest)
    gtest_discover_tests(${test_name})
endforeach()