add_executable(test_vec test_vec.cpp)
target_link_libraries(test_vec PRIVATE Catch2::Catch2WithMain ns::linalg ns::core)
catch_discover_tests(test_vec)
