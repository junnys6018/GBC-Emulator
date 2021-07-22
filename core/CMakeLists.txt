file(GLOB_RECURSE SOURCES src/*)

add_library(core STATIC ${SOURCES})

source_group(" " REGULAR_EXPRESSION "src/.*")
source_group(util REGULAR_EXPRESSION "src/util/.*")

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
        <bit>
        <array>
        <map>
        <set>
        <sstream>
        <string>
        <unordered_map>
        <unordered_set>
        <vector>
        <cmath>
        <cstdlib>
        <cstring>
)