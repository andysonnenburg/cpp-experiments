cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME cpp-experiments)
project("${PROJECT_NAME}")

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

enable_testing()
find_package(GTest REQUIRED)

set(PROJECT_TEST_NAME "${PROJECT_NAME}_test")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++11")

include_directories(include)
file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})

file(GLOB_RECURSE TEST_FILES "${PROJECT_SOURCE_DIR}/test/*.cpp")
add_executable("${PROJECT_TEST_NAME}" ${TEST_FILES})
target_link_libraries(cpp-experiments_test gtest_main)

add_test(all_tests "${PROJECT_TEST_NAME}")
