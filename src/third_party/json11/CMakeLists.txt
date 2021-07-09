set(json11_BASE ${CMAKE_CURRENT_SOURCE_DIR})
set(json11_SOURCE_DIR ${json11_BASE}/dropbox)

if(NOT TARGET json11)
  add_library(json11 OBJECT ${json11_SOURCE_DIR}/json11.cpp)
  target_compile_options(json11 PRIVATE -fno-rtti -fno-exceptions)
  target_include_directories(json11 SYSTEM PUBLIC ${json11_BASE})
  if(BUILD_SHARED_LIBS)
    set_target_properties(json11 PROPERTIES POSITION_INDEPENDENT_CODE ON)
  endif()

  install(FILES
      ${json11_SOURCE_DIR}/json11.hpp
      ${json11_SOURCE_DIR}/LICENSE.txt
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ersap/third_party/dropbox
    COMPONENT Ersap_Devel
  )
endif()
