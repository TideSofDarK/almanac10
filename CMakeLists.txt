cmake_minimum_required(VERSION 3.10)
project(almanac10 C)

set(CMAKE_C_STANDARD 11)

if (UNIX)
    set(UNIX_FLAGS "m; dl")
endif (UNIX)

# cglm
set(CGLM_INCLUDE_DIR "" CACHE FILEPATH "sets cglm include path")
include_directories(${CGLM_INCLUDE_DIR})

# nuklear
set(NUKLEAR_INCLUDE_DIR "" CACHE FILEPATH "sets nuklear include path")
include_directories(${NUKLEAR_INCLUDE_DIR})

# GLAD
set(GLAD_INCLUDE_DIR "" CACHE FILEPATH "sets GLAD include path")
include_directories(${GLAD_INCLUDE_DIR})

find_package(assimp REQUIRED)
find_package(glfw3 REQUIRED)
find_package(lua REQUIRED)

file(GLOB almanac10_src
        "engine/*.h"
        "engine/*.c"
        )
add_executable(almanac10 ${almanac10_src} almanac10.rc)

target_link_libraries(almanac10
        assimp
        glfw
        lua
        ${UNIX_FLAGS}
        )

# Copy Lua scripts to assets directory
add_custom_command(TARGET almanac10 PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scripts/ $<TARGET_FILE_DIR:almanac10>/assets/scripts/)