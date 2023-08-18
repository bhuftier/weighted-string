set(SOURCES
    "wstr/weighted_element.hpp"
    "wstr/weighted_string.hpp"
    "wstr/dna_weighted_string.hpp"
)

add_library(wstr ${SOURCES})

# Needs c++17 for inline constexpr variables
set_target_properties(wstr PROPERTIES CXX_STANDARD 17)
set_target_properties(wstr PROPERTIES CXX_STANDARD_REQUIRED ON)

set_target_properties(wstr PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(wstr PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/wstr")
target_include_directories(wstr INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")