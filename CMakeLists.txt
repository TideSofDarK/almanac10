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
        C:/workspace/libs/LuaAutoC/include
)

link_directories(
        C:/workspace/libs/LuaAutoC/lib
)

add_executable(almanac10
        camera.c
        camera.h
        config.c
        config.h
        creature.c
        creature.h
        debug.c
        debug.h
        game.c
        game.h
        glad.c
        grid.c
        grid.h
        input.c
        input.h
        script.h
        script.c
        main.c
        mesh.c
        mesh.h
        model.c
        model.h
        nuklear_init.h
        projectile.c
        projectile.h
        renderer.c
        renderer.h
        shader.c
        shader.h
        sprite.c
        sprite.h
        stb_image.c
        stb_image.h
        texture.c
        texture.h
        transform.c
        transform.h
        ui.c
        ui.h
        util.c
        util.h
        vector.h
        vertices.c
        vertices.h
        world.c
        world.h
        almanac10.rc object.c object.h)

target_link_libraries(almanac10
        assimp
        glfw
        lautoc
        lua
        )