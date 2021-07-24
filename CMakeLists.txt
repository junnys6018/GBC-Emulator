cmake_minimum_required(VERSION 3.16)
project(GBC-Emulator)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release") 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ---------- configuration options ----------

option(GBC_USE_CXX_20 "Compile with c++ 20" ON)

# ---------- specify the C++ standard ----------
if (GBC_USE_CXX_20)
    set(CMAKE_CXX_STANDARD 20)
    add_compile_definitions(GBC_HAS_CXX_20)
else ()
    set(CMAKE_CXX_STANDARD 17)
endif()
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

# Move libraries to its own folder
set_property(TARGET spdlog PROPERTY FOLDER "vendor")

# ---------- Add Subdirectories ----------

add_subdirectory(core)
add_subdirectory(application)
add_subdirectory(tests)