set(CMAKE_CXX_CLANG_TIDY "")

add_subdirectory(rapidjson)
add_subdirectory(json11)

add_library(concurrentqueue INTERFACE IMPORTED GLOBAL)
set_target_properties(concurrentqueue PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/concurrent_queue)

add_library(thread_pool INTERFACE IMPORTED GLOBAL)
set_target_properties(thread_pool PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/thread_pool)

add_library(optionparser INTERFACE IMPORTED GLOBAL)
set_target_properties(optionparser PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/optionparser)
