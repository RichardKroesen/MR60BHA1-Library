cmake_minimum_required(VERSION 3.14)
project(YourLibraryTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(lib)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)
#  Checksum Test
add_executable(
  checksum_test
  ../tests/MR60BHA1_checksum_test.cpp
)

target_link_libraries(
  checksum_test
  GTest::gtest_main
)

gtest_discover_tests(checksum_test)

# Frame Test
add_executable(
  frame_test
  ../tests/MR60BHA1_frame_test.cpp
)
target_link_libraries(
  frame_test
  GTest::gtest_main
)
gtest_discover_tests(frame_test)