file(GLOB_RECURSE SOURCES src/*)

add_library(core STATIC ${SOURCES})

target_include_directories(core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/spdlog/include
)
target_compile_definitions(core PUBLIC SPDLOG_COMPILED_LIB)

target_precompile_headers(core
    PRIVATE
        <algorithm>
        <cassert>
        <chrono>
        <functional>
        <iostream>
        <memory>
        <utility>
        <cmath>
        <array>
        <map>
        <set>
        <sstream>
        <string>
        <unordered_map>
        <unordered_set>
        <vector>
)