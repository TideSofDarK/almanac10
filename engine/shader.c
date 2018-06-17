#include "shader.h"

#include <stdio.h>
#include <malloc.h>

#include <glad/glad.h>
#include <assimp/types.h>

#include "util.h"

#define PROGRAM 0
#define VERTEX 1
#define GEOMETRY 2
#define FRAGMENT 3

static inline void load_shader_from_file(char **dest, const char *shader) {
    char *full_path = NULL;
    asprintf(&full_path, "assets/shaders/%s", shader);

    *dest = load_string_from_file(full_path);
    free(full_path);
}

static inline void check_compile_errors(unsigned int shader, int type) {
    int success;
    char infoLog[1024];
    if (type != PROGRAM) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("%i failed: %s\n", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("Linking %i failed: %s\n", type, infoLog);
        }
    }
}

void construct_shader(Shader **_shader, const char *vertex_shader_filename, const char *fragment_shader_filename,
                      const char *geometry_shader_filename) {
    *_shader = malloc(sizeof(Shader));
    Shader *shader = *_shader;

    shader->vertex_shader_source = NULL;
    load_shader_from_file(&shader->vertex_shader_source, vertex_shader_filename);
    shader->fragment_shader_source = NULL;
    load_shader_from_file(&shader->fragment_shader_source, fragment_shader_filename);
    shader->geometry_shader_source = NULL;
    if (geometry_shader_filename != NULL) {
        shader->geometry_shader_source = load_string_from_file(geometry_shader_filename);
    }

    int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const GLchar *const *) &shader->vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    check_compile_errors(vertex_shader, VERTEX);

    int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const GLchar *const *) &shader->fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    check_compile_errors(fragment_shader, FRAGMENT);

    int geometry_shader = 0;
    if (geometry_shader_filename != NULL) {
        geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry_shader, 1, (const GLchar *const *) &shader->geometry_shader_source, NULL);
        glCompileShader(geometry_shader);
        check_compile_errors(geometry_shader, GEOMETRY);
    }

    shader->ID = glCreateProgram();
    glAttachShader(shader->ID, vertex_shader);
    glAttachShader(shader->ID, fragment_shader);
    if (geometry_shader_filename != NULL)
        glAttachShader(shader->ID, geometry_shader);
    glLinkProgram(shader->ID);
    check_compile_errors(shader->ID, PROGRAM);

    glDetachShader(shader->ID, vertex_shader);
    glDetachShader(shader->ID, fragment_shader);
    if (geometry_shader_filename != NULL)
        glDetachShader(shader->ID, geometry_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    if (geometry_shader_filename != NULL)
        glDeleteShader(geometry_shader);
}

void destruct_shader(Shader **_shader) {
    Shader *shader = *_shader;

    glDeleteProgram(shader->ID);

    free(shader->vertex_shader_source);
    free(shader->fragment_shader_source);
    if (shader->geometry_shader_source)
        free(shader->geometry_shader_source);

    free(*_shader);
    *_shader = NULL;
}

void use_shader(Shader *shader) {
    glUseProgram(shader->ID);
}

void set_uniform_vec2(Shader *shader, const char *name, vec2 value) {
    vec2 cvalue;
    glm_vec_copy(value, cvalue);
    glUniform2f(glGetUniformLocation(shader->ID, name), value[0], value[1]);
}

void set_uniform_vec3(Shader *shader, const char *name, vec3 value) {
    vec3 cvalue;
    glm_vec_copy(value, cvalue);
    glUniform3f(glGetUniformLocation(shader->ID, name), cvalue[0], cvalue[1], cvalue[2]);
}

void set_uniform_mat4(Shader *shader, const char *name, mat4 value) {
    mat4 cvalue;
    glm_mat4_copy(value, cvalue);
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, (float *) value);
}

void set_uniform_vec4(Shader *shader, const char *name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(shader->ID, name), x, y, z, w);
}

void set_uniform_float(Shader *shader, const char *name, float value) {
    glUniform1f(glGetUniformLocation(shader->ID, name), value);
}

void set_uniform_int(Shader *shader, const char *name, int value) {
    glUniform1i(glGetUniformLocation(shader->ID, name), value);
}