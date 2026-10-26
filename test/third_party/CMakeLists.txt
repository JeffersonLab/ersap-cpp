FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  URL_HASH SHA256=f179ec217f9b3b3f3c6e8b02d3e7eda997b49e4ce26d6b235c9053bec9c0bf9f
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
