cmake_minimum_required(VERSION 4.2)

project(
  ns
  VERSION      0.1.0
  LANGUAGES    CXX
  HOMEPAGE_URL "https://github.com/abzrg/ns"
  DESCRIPTION  "NS (Numerical Solvers) - linear and nonlinear solvers (C++23)"
)

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build generator settings
include(cmake/build_generator.cmake)

# The main project source directory
add_subdirectory(src)

# Testing enabled by setting BUILD_TESTING=ON
include(cmake/testing.cmake)

# Install configurations and findpackage settings
# Usage:
#   find_package(ns CONFIG REQUIRED)
#   target_link_libraries(myapp PRIVATE ns::ns)
include(cmake/findpackage.cmake)

# Pkgconfig configurations
# Usage:
#   pkg-config --cflags ns
#   g++ main.cpp $(pkg-config --cflags ns)
include(cmake/pkgconfig.cmake)
