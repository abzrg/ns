if(BUILD_TESTING)
  set(CMAKE_CTEST_ARGUMENTS
      "--output-on-failure" "--output-junit" "junit.xml")
  include(CTest)

  # Catch2
  Include(FetchContent)
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
