cmake_minimum_required(VERSION 3.16)
project(GBC-Emulator)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release") 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ---------- specify the C++ standard ----------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---------- Global Configuration Settings ----------

add_compile_definitions($<$<CONFIG:Debug>:CONFIGURATION_DEBUG>)
add_compile_definitions($<$<CONFIG:Release>:CONFIGURATION_RELEASE>)

add_compile_definitions($<$<PLATFORM_ID:Windows>:PLATFORM_WINDOWS>)
add_compile_definitions($<$<PLATFORM_ID:Linux>:PLATFORM_LINUX>)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    message(FATAL_ERROR "Unknown Compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# ---------- Add Third Party Libraries ----------

# spdlog
set(SPDLOG_ENABLE_PCH ON CACHE BOOL " " FORCE)
add_subdirectory(vendor/spdlog)

# ---------- Add Subdirectories ----------

add_subdirectory(core)
add_subdirectory(application)
add_subdirectory(tests)