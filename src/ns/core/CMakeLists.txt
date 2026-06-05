add_library(ns_core INTERFACE)
add_library(ns::core ALIAS ns_core)

# So that we can link against ns::core instead of ns::ns_core
set_target_properties(ns_core PROPERTIES EXPORT_NAME core)

target_include_directories(
  ns_core
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(
  ns_core
  INTERFACE
    cxx_std_23
)
