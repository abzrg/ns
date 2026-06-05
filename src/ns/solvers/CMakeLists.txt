add_library(ns_solvers INTERFACE)
add_library(ns::solvers ALIAS ns_solvers)

set_target_properties(ns_solvers PROPERTIES EXPORT_NAME solvers)

target_link_libraries(
  ns_solvers
  INTERFACE
    ns_linalg
)
