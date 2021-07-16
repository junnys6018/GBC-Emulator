file(GLOB_RECURSE SOURCES src/*)

add_executable(application ${SOURCES})

target_include_directories(application PUBLIC ${CMAKE_SOURCE_DIR}/core/src ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(application PUBLIC core)

target_precompile_headers(application REUSE_FROM core)