if(BUILD_TESTING)
  set(CMAKE_CTEST_ARGUMENTS
      "--output-on-failure" "--output-junit" "junit.xml")
  include(CTest)

  # --- Fetch and build Catch2

  Include(FetchContent)

  # By default FetchContent does a git fetch to verify the specified tag ref hasn't moved, everytime
  # CMake reconfigures. The following skips that check entirely once it has been downloaded once.
  set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

  # Be verbose when fetching contents
  set(FETCHCONTENT_QUIET OFF)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.0.1 # or a later release
    GIT_CONFIG     "advice.detachedhead=false"
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
  )
  FetchContent_MakeAvailable(Catch2)
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
  include(Catch)

  add_subdirectory(tests)
endif()
