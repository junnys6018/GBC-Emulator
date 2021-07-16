file(GLOB_RECURSE SOURCES src/*)

add_executable(tests ${SOURCES})

target_include_directories(tests PUBLIC ${CMAKE_SOURCE_DIR}/core/src ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(tests PUBLIC core)

target_precompile_headers(tests REUSE_FROM core)	