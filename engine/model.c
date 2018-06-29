#include "model.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "util.h"

#define PROPS_PATH "./assets/models/props/%s/"

/* TODO: Asset precache manager */
static Model **precached_models = NULL;

void process_mesh(const char *folder, const aiScene *ai_scene, aiMesh *ai_mesh, Mesh **_mesh)
{
    /* Vertices, normals, texcoords, indices */
    Vertex *vertices = NULL;
    int *indices = NULL;
    for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm_vec_copy((vec3){ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z}, vertex.pos);

        if (ai_mesh->mNormals && ai_mesh->mNormals[i].x)
        {
            glm_vec_copy((vec3){ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z},
                         vertex.normal);
        }
        else
        {
            glm_vec_zero(vertex.normal);
        }

        if (ai_mesh->mTextureCoords[0])
        {
            glm_vec_copy((vec2){ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y}, vertex.tex_coords);
        }
        else
        {
            glm_vec_copy((vec2){0.0f, 0.0f}, vertex.tex_coords);
        }

        vector_push_back(vertices, vertex);
    }
    for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
    {
        aiFace face = ai_mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            vector_push_back(indices, face.mIndices[j]);
        }
    }

    aiString str;
    if (ai_mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
        aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &str, NULL, NULL, NULL, NULL, NULL, NULL);
    }

    char *texture_path = NULL;
    asprintf(&texture_path, "%s%s", folder, str.data);

    construct_mesh(vertices, indices, get_texture(texture_path), _mesh);

    free(texture_path);
}

void process_node(const char *folder, Model *model, aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        Mesh *mesh = NULL;
        process_mesh(folder, scene, scene->mMeshes[node->mMeshes[i]], &mesh);
        vector_push_back(model->meshes, mesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(folder, model, node->mChildren[i], scene);
    }
}

static inline void construct_model(Model **_model, const char *folder, const char *name)
{
    *_model = malloc(sizeof(Model));
    Model *model = *_model;

    model->name = name;
    model->meshes = NULL;

    char *obj_path = NULL;
    asprintf(&obj_path, "%s%s.obj", folder, name);

    const aiScene *ai_scene = aiImportFile(obj_path, aiProcess_Triangulate | aiProcess_FlipUVs);
    process_node(folder, model, ai_scene->mRootNode, ai_scene);

    aiReleaseImport(ai_scene);

    vector_push_back(precached_models, model);

    free(obj_path);
}

static inline void destruct_model(Model **_model)
{
    Model *model = *_model;

    printf("[Engine] Freeing model: %s\n", model->name);

    for (unsigned int i = 0; i < vector_size(model->meshes); i++)
        destruct_mesh(&model->meshes[i]);
    vector_free(model->meshes);
    model->meshes = NULL;

    free(*_model);
    *_model = NULL;
}

int free_precached_models()
{
    int count = 0;
    for (int i = 0; i < (int)vector_size(precached_models); i++)
    {
        Model *model = precached_models[i];
        if (model != NULL)
        {
            destruct_model(&model);
            count++;
        }
    }
    return count;
}

void free_precached_model(Model *model)
{
    if (model == NULL)
        return;

    for (int i = 0; i < (int)vector_size(precached_models); i++)
    {
        if (strcmp(model->name, precached_models[i]->name) == 0)
        {
            destruct_model(&model);

            vector_erase(precached_models, i);

            break;
        }
    }
}

Model *get_model(const char *folder, const char *name)
{
    Model *model = NULL;

    /* TODO: Respect folder name */
    for (size_t i = 0; i < vector_size(precached_models); ++i)
    {
        if (strcmp(precached_models[i]->name, name) == 0)
        {
            return precached_models[i];
        }
    }

    construct_model(&model, folder, name);

    return model;
}

Model *get_prop_model(const char *name)
{
    Model *model = NULL;
    char *folder = NULL;
    asprintf(&folder, PROPS_PATH, name);

    model = get_model(folder, name);

    free(folder);

    return model;
}