# Example:
#   find_package(ns CONFIG REQUIRED)
#   target_link_libraries(my_app PRIVATE ns::ns)

install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/src/ns
  DESTINATION include
  PATTERN "CMakeLists.txt" EXCLUDE
)

# Install the following targets and remember them as part of an export set called nsTargets.
# Record
# - include directories
# - interface dependencies
# - compile features
# - target names
# Note: This does NOT install headers. It only installs CMake metadata for targets.
install(
  TARGETS
    ns_core
    ns_linalg
    ns_solvers
    ns_ns
  EXPORT nsTargets
)

# Generate nsTargets.cmake, which contains
#
#   add_library(ns::core INTERFACE IMPORTED)
#   set_target_properties(...)
#   ...
#
# so downstream users get:
# - ns::ns
# - ns::core
# - ns::linalg
# - ns::cg
# Note: the following does not copy files. It simply exports/generates CMake targets, not filesystem
# content.
install(
  EXPORT nsTargets
  FILE nsTargets.cmake
  NAMESPACE ns::
  DESTINATION lib/cmake/ns
)

# This module provides helpers to generate:
# - nsConfig.cmake
# - nsConfigVersion.cmake
# These are required for: find_package(ns CONFIG REQUIRED)
include(CMakePackageConfigHelpers)

# It converts the template within the nsConfig.cmake.in,
#
#   @PACKAGE_INIT@
#   include("${CMAKE_CURRENT_LIST_DIR}/nsTargets.cmake")
#
# into a real file:
#
#   nsConfig.cmake
#
# installed alongside your targets.
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nsConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/nsConfig.cmake
  # Generate correct relative paths based on the fact that this package will be installed under
  # lib/cmake/ns
  INSTALL_DESTINATION lib/cmake/ns
)

# Create nsConfigVersion.cmake, which enforces: version compatibility and find_package version
# rules.
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/nsConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  # The following means:
  # If you install ns-v1.4.2, then:
  #   find_package(ns 1.2 REQUIRED) # works
  #   find_package(ns 2.0 REQUIRED) # fails
  COMPATIBILITY SameMajorVersion
)

# Install (copy) the entry point for: find_package(ns CONFIG)
#   <install_prefix>/cmake/ns/
#     nsConfig.cmake
#     nsConfigVersion.cmake
#     nsTargets.cmake
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nsConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nsConfigVersion.cmake
  DESTINATION lib/cmake/ns
)

# Export build-tree usage
# This is only for using the project without installing it. For instance,
#   add_subdirectory(ns)
#   target_link_libraries(myapp PRIVATE ns::cg)
# without install().
export(
  EXPORT nsTargets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/nsTargets.cmake
  NAMESPACE ns::
)
