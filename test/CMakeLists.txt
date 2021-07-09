#----------------------------------------------------------------------
# Global settings
#
unset(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)

add_subdirectory(third_party)


#----------------------------------------------------------------------
# Unit tests
#
set(ERSAP_UNIT_TESTS
  composition_compiler
  data_utils
  engine_data
  engine_data_type
  utils
)

foreach(name IN LISTS ERSAP_UNIT_TESTS)
  add_executable(test_${name} ${name}_test.cpp)
  add_test(NAME test_${name} COMMAND test_${name} CONFIGURATIONS $<CONFIG> "")
  set_tests_properties(test_${name} PROPERTIES LABELS "unit")
  target_include_directories(test_${name} PRIVATE "${PROJECT_SOURCE_DIR}/src")
  target_link_libraries(test_${name} PRIVATE Ersap::ersap GTest::GMock)
endforeach()


#----------------------------------------------------------------------
# Standalone executable tests
#
add_executable(test_service_loader service_loader_test.cpp)
target_link_libraries(test_service_loader PRIVATE Ersap::ersap ${CMAKE_DL_LIBS} GTest::GMock)
target_include_directories(test_service_loader PRIVATE "${PROJECT_SOURCE_DIR}/src")
