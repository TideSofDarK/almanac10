#include "mesh.h"

#include <glad/glad.h>

void construct_mesh(Vertex *vertices, int *indices, Texture *texture, Mesh **_mesh) {
    *_mesh = malloc(sizeof(Mesh));
    Mesh *mesh = *_mesh;

    mesh->vertices = vertices;
    mesh->indices = indices;
    mesh->texture = texture;

    glGenVertexArrays(1, &mesh->render_data.VAO);
    glGenBuffers(1, &mesh->render_data.VBO);
    glGenBuffers(1, &mesh->render_data.EBO);

    glBindVertexArray(mesh->render_data.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->render_data.VBO);
    glBufferData(GL_ARRAY_BUFFER, vector_size(mesh->vertices) * sizeof(Vertex), &mesh->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->render_data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vector_size(mesh->indices) * sizeof(unsigned int), &mesh->indices[0],
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex_coords));

    glBindVertexArray(0);
}

void destruct_mesh(Mesh **_mesh) {
    Mesh *mesh = *_mesh;

    vector_free(mesh->vertices);
    mesh->vertices = NULL;

    vector_free(mesh->indices);
    mesh->indices = NULL;

    free(*_mesh);
    *_mesh = NULL;
}