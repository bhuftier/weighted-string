set(SOURCES
    "wstr/weighted_element.hpp"
    "wstr/weighted_string.cpp"
    "wstr/weighted_string_from_file.cpp"
)

add_library(wstr ${SOURCES})
set_target_properties(wstr PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(wstr PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/wstr")
target_include_directories(wstr INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")