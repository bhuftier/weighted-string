set(TEST_SOURCES
    "test_weighted_element.cpp"
    "test_weighted_string.cpp"
    "test_dna_weighted_string.cpp"
    # "test_readme_example.cpp"
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# To enable testing wth cmake
enable_testing()

# Executable
add_executable(wstr-test ${TEST_SOURCES})

# GoogleTest requires at least C++14
set_target_properties(wstr-test PROPERTIES CXX_STANDARD 17)
set_target_properties(wstr-test PROPERTIES CXX_STANDARD_REQUIRED ON)

target_link_libraries(wstr-test PUBLIC GTest::gtest_main)
target_link_libraries(wstr-test PUBLIC wstr)

# Configuration file
configure_file(config.h.in config.h)
target_include_directories(wstr-test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# Enable CMake's test runner to discover the tests included in the binary
include(GoogleTest)
gtest_discover_tests(wstr-test)
