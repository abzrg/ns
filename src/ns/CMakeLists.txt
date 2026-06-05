add_subdirectory(core)
add_subdirectory(linalg)
add_subdirectory(solvers)

# ---

add_library(ns_ns INTERFACE)
add_library(ns::ns ALIAS ns_ns)

set_target_properties(ns_ns PROPERTIES EXPORT_NAME ns)

target_link_libraries(
  ns_ns
  INTERFACE
    ns_core
    ns_linalg
    ns_solvers
)
