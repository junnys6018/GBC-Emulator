file(GLOB_RECURSE SOURCES src/*)

add_executable(application ${SOURCES})

target_include_directories(application PUBLIC
    ${CMAKE_SOURCE_DIR}/core/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor/spdlog/include
)
target_link_libraries(application PUBLIC core spdlog::spdlog)
target_compile_definitions(application PUBLIC SPDLOG_COMPILED_LIB)

target_precompile_headers(application REUSE_FROM core)