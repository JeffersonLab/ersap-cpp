FetchContent_Declare(
  xmsg
  GIT_REPOSITORY https://github.com/JeffersonLab/xmsg-cpp.git
  GIT_TAG origin/master
)

if(FETCHCONTENT_SOURCE_DIR_XMSG)
  set(XMSG_BUILD_TESTS ${ERSAP_BUILD_TESTS} CACHE BOOL "Build tests")
else()
  set(CMAKE_CXX_CLANG_TIDY "")
endif()

FetchContent_MakeAvailable(xmsg)
