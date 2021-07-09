FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.8.1.zip
  URL_HASH SHA256=927827c183d01734cc5cfef85e0ff3f5a92ffe6188e0d18e909c5efebf28a0c7
)

FetchContent_GetProperties(googletest)

if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)

  set(CMAKE_CXX_CLANG_TIDY "")
  set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL YES)

  option(INSTALL_GTEST "Install googletest." OFF)
  set(BUILD_SHARED_LIBS OFF)

  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

  mark_as_advanced(INSTALL_GTEST BUILD_GMOCK)
endif()

if(NOT TARGET GTest::GTest)
  add_library(GTest::GTest ALIAS gtest)
endif()
if(NOT TARGET GTest::GMock)
  add_library(GTest::GMock ALIAS gmock)
endif()
