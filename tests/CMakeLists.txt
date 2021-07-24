file(GLOB_RECURSE SOURCES src/*)
set(CATCH vendor/catch2/catch_amalgamated.cpp vendor/catch2/catch_amalgamated.hpp)

add_executable(tests ${SOURCES} ${CATCH})

source_group(vendor/catch2 FILES ${CATCH})

target_include_directories(tests PUBLIC
    ${CMAKE_SOURCE_DIR}/core/src 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/catch2
)
target_link_libraries(tests PUBLIC 
    core
    spdlog::spdlog
)
target_compile_definitions(tests PUBLIC SPDLOG_COMPILED_LIB)
target_precompile_headers(tests REUSE_FROM core)

# set working dir when debugging on visual studio
set_property(TARGET tests PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")