cmake_minimum_required(VERSION 3.10)
project(almanac10 C)

set(CMAKE_C_STANDARD 11)

include_directories(
        .
        C:/Users/tides/Downloads/cglm-master/include
        C:/Users/tides/Downloads/glad-0.1.20/GL/include
        C:/Users/tides/Downloads/nuklear-master
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
        world.h)

target_link_libraries(almanac10
        PUBLIC libglfw3
        PUBLIC libassimp.dll)