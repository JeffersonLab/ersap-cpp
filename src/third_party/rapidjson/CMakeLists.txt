FetchContent_Declare(
  rapidjson
  URL https://github.com/Tencent/rapidjson/archive/v1.1.0.tar.gz
  URL_HASH SHA256=bf7ced29704a1e696fbccf2a2b4ea068e7774fa37f6d7dd4039d0787f8bed98e
)

FetchContent_GetProperties(rapidjson)

if(NOT rapidjson_POPULATED)
  FetchContent_Populate(rapidjson)

  add_library(rapidjson INTERFACE IMPORTED GLOBAL)
  set_target_properties(rapidjson PROPERTIES
    INTERFACE_COMPILE_DEFINITIONS RAPIDJSON_HAS_STDSTRING)
  set_target_properties(rapidjson PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${rapidjson_SOURCE_DIR}/include)
endif()
