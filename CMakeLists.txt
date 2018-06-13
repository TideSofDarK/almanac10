cmake_minimum_required(VERSION 3.10)
project(almanac10 C)

set(CMAKE_C_STANDARD 11)

find_package(assimp REQUIRED)
find_package(glfw3 REQUIRED)
find_package(lua REQUIRED)

include_directories(
        .
        C:/workspace/libs/cglm/include
        C:/workspace/libs/nuklear
        C:/workspace/libs/glad/include
)

file(GLOB almanac10_src
        "engine/*.h"
        "engine/*.c"
        )
add_executable(almanac10 ${almanac10_src} almanac10.rc)

target_link_libraries(almanac10
        assimp
        glfw
        lua
        )

# Copy Lua scripts to assets directory
add_custom_command(TARGET almanac10 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scripts/ $<TARGET_FILE_DIR:almanac10>/assets/scripts/)