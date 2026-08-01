FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
  URL_HASH SHA256=40d4ec942217dcc84a9ebe2a68584ada7d4a33a8ee958755763278ea1c5e18ff
)

set(CMAKE_CXX_CLANG_TIDY "")
set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL YES)

option(INSTALL_GTEST "Install googletest." OFF)
set(BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(googletest)

mark_as_advanced(INSTALL_GTEST BUILD_GMOCK)

if(NOT TARGET GTest::GTest)
  add_library(GTest::GTest ALIAS gtest)
endif()
if(NOT TARGET GTest::GMock)
  add_library(GTest::GMock ALIAS gmock)
endif()
