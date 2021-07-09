cmake_minimum_required(VERSION 3.5.1)
project(ERSAP LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release MinSizeRel RelWithDebInfo)
endif()

option(ERSAP_BUILD_TESTS "Build tests" ON)
option(ERSAP_BUILD_DOCS "Build docs" ON)
option(ERSAP_USE_SYSTEM_XMSG "Use the version of xMsg installed in the system" OFF)

if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
endif()

include(GNUInstallDirs)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()


find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE PATH "")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra)

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  file(RELATIVE_PATH relDir
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
    ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
  )
  set(CMAKE_INSTALL_RPATH @loader_path @loader_path/${relDir})
endif()


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/ThirdParty" CACHE PATH
  "Directory under which to collect all populated content")
if(${CMAKE_VERSION} VERSION_LESS 3.11)
  include(cmake/External/FetchContent.cmake)
else()
  include(FetchContent)
endif()

if(ERSAP_BUILD_TESTS)
  enable_testing()
endif()

if(ERSAP_USE_SYSTEM_XMSG)
  find_package(xMsg CONFIG REQUIRED)
else()
  add_subdirectory(src/third_party/xmsg)
endif()

add_subdirectory(src/third_party)

string(CONCAT PKGCONF_REQ_PUB "xmsg")


add_subdirectory(src)

if(ERSAP_BUILD_DOCS)
  add_subdirectory(doc)
endif()
if(ERSAP_BUILD_TESTS)
  add_subdirectory(test)
endif()


# package configuration
set(CMAKE_INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/ersap")

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ersap-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ersap-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ersap-config.cmake
  DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  COMPONENT Ersap_Devel)


# export targets
export(EXPORT ErsapTargets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/ersap-targets.cmake
  NAMESPACE Ersap::)

install(EXPORT ErsapTargets
  FILE ersap-targets.cmake
  NAMESPACE Ersap::
  DESTINATION ${CMAKE_INSTALL_CONFIGDIR}
  COMPONENT Ersap_Devel)


# pkg-config
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ersap.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/ersap.pc
  @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ersap.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  COMPONENT Ersap_Devel)


# uninstall target
configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ersap_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
