#include "terrain.h"
#include "mesh.h"

#include <glad/glad.h>

void construct_terrain(Terrain **_terrain, int grid_size) {
    *_terrain = malloc(sizeof(Terrain));
    Terrain *terrain = *_terrain;

    init_transform(&terrain->transform);
    transform_scale(&terrain->transform, (float) grid_size);

    terrain->grid_size = grid_size;
    terrain->vertices = NULL;
    terrain->indices = NULL;

    /* Vertices */
    int g = grid_size + 1;
    for (int x = 0; x < g; ++x) {
        for (int z = 0; z < g; ++z) {
            Vertex vertex;

            glm_vec_copy((vec3) {(((float) x / (float) grid_size) - 0.5f) * 2.0f, 0.0f,
                                 (((float) z / (float) grid_size) - 0.5f) * 2.0f}, vertex.pos);
            vertex.tex_coords[0] = vertex.pos[0];
            vertex.tex_coords[1] = vertex.pos[2];

            vector_push_back(terrain->vertices, vertex);
        }
    }

    /* Indices */
    for (int z = 0; z < grid_size; ++z) {
        for (int x = 0; x < grid_size; ++x) {
            int i = x + (z * grid_size) + z;
            vector_push_back(terrain->indices, i + g);
            vector_push_back(terrain->indices, i);
            vector_push_back(terrain->indices, i + g + 1);
            vector_push_back(terrain->indices, i);
            vector_push_back(terrain->indices, i + g + 1);
            vector_push_back(terrain->indices, i + 1);
        }
    }

//    for (size_t i = 0; i < vector_size(terrain->indices); i += 6)
//    {
//        printf("1)            %i\n", terrain->indices[i]);
//        printf("  2)          %i\n", terrain->indices[i+1]);
//        printf("    3)        %i\n", terrain->indices[i+2]);
//        printf("      4)      %i\n", terrain->indices[i+3]);
//        printf("        5)    %i\n", terrain->indices[i+4]);
//        printf("          6)  %i\n", terrain->indices[i+5]);
//    }

    glGenVertexArrays(1, &terrain->render_data.VAO);
    glGenBuffers(1, &terrain->render_data.VBO);
    glGenBuffers(1, &terrain->render_data.EBO);

    rebuild_terrain(terrain);
}

void destruct_terrain(Terrain **_terrain) {
    Terrain *terrain = *_terrain;

    vector_free(terrain->vertices);
    terrain->vertices = NULL;

    vector_free(terrain->indices);
    terrain->indices = NULL;

    free(*_terrain);
    *_terrain = NULL;
}

void rebuild_terrain(Terrain *terrain) {
    glBindVertexArray(terrain->render_data.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->render_data.VBO);
    glBufferData(GL_ARRAY_BUFFER, vector_size(terrain->vertices) * sizeof(Vertex), &terrain->vertices[0],
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->render_data.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vector_size(terrain->indices) * sizeof(unsigned int), &terrain->indices[0],
                 GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, tex_coords));

    glBindVertexArray(0);
}