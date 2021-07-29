
# ---------- Add Third Party Libraries ----------

# glad
add_subdirectory(vendor/glad)

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
add_subdirectory(vendor/glfw)

# imgui
add_subdirectory(vendor/imgui)

# Move libraries to its own folder
set_property(TARGET glad glfw imgui PROPERTY FOLDER "vendor")

# ---------- Main Application ----------

file(GLOB_RECURSE SOURCES src/*)
add_executable(application ${SOURCES})

source_group(" " REGULAR_EXPRESSION "src/.*")
source_group(windowing REGULAR_EXPRESSION "src/windowing/.*")
source_group(debug REGULAR_EXPRESSION "src/debug/.*")
source_group(opengl REGULAR_EXPRESSION "src/opengl/.*")

target_include_directories(application PUBLIC
    ${CMAKE_SOURCE_DIR}/core/src
    ${CMAKE_SOURCE_DIR}/vendor/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui_club
)
target_link_libraries(application PUBLIC 
    core
    spdlog::spdlog
    glad
    glfw
    imgui
)
target_compile_definitions(application PUBLIC SPDLOG_COMPILED_LIB)

target_precompile_headers(application REUSE_FROM core)

# set working dir when debugging on visual studio
set_property(TARGET application PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")