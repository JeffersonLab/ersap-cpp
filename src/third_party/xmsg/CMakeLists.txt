FetchContent_Declare(
  xmsg
  GIT_REPOSITORY https://github.com/JeffersonLab/xmsg-cpp.git
  GIT_TAG origin/master
)

FetchContent_GetProperties(xmsg)

if(NOT xmsg_POPULATED)
  FetchContent_Populate(xmsg)

  if(FETCHCONTENT_SOURCE_DIR_XMSG)
    set(XMSG_BUILD_TESTS ${ERSAP_BUILD_TESTS} CACHE BOOL "Build tests")
  else()
    set(CMAKE_CXX_CLANG_TIDY "")
  endif()

  add_subdirectory(${xmsg_SOURCE_DIR} ${xmsg_BINARY_DIR})
endif()
