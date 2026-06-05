add_library(ns_linalg INTERFACE)
add_library(ns::linalg ALIAS ns_linalg)

set_target_properties(ns_linalg PROPERTIES EXPORT_NAME linalg)

target_link_libraries(
  ns_linalg
  INTERFACE
    ns_core
)
