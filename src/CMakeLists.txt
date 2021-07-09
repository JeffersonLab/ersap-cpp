set(ERSAP_SRC
  base.cpp
  component.cpp
  composition.cpp
  container.cpp
  container_report.cpp
  data_utils.cpp
  dpe.cpp
  dpe_report.cpp
  engine_data.cpp
  engine_data_type.cpp
  json_report.cpp
  service.cpp
  service_engine.cpp
  service_report.cpp
  utils.cpp
)

set(ERSAP_STD_SRC
  stdlib/event_reader_service.cpp
  stdlib/event_writer_service.cpp
  stdlib/streaming_service.cpp
  stdlib/json_utils.cpp
)

add_library(ersap ${ERSAP_SRC} ${ERSAP_STD_SRC} $<TARGET_OBJECTS:json11>)
target_link_libraries(ersap
  PRIVATE rapidjson concurrentqueue thread_pool ${CMAKE_DL_LIBS}
  PUBLIC xMsg::xmsg
)

get_target_property(JSON11_INCLUDEDIR json11 INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(ersap PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(ersap SYSTEM PUBLIC
  $<BUILD_INTERFACE:${JSON11_INCLUDEDIR}>
)

add_library(Ersap::ersap ALIAS ersap)

install(TARGETS ersap EXPORT ErsapTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT Ersap_Runtime
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ersap
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT Ersap_Devel
)


add_executable(c_dpe run_dpe.cpp)
target_link_libraries(c_dpe optionparser ersap)

install(TARGETS c_dpe
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/ersap
  COMPONENT Ersap_Runtime
)
install(FILES ${PROJECT_SOURCE_DIR}/scripts/unix/c_dpe
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
              GROUP_READ GROUP_EXECUTE
              WORLD_READ WORLD_EXECUTE
  DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT Ersap_Runtime
)
