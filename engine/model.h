#pragma once

#include "mesh.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define TYPEDEF(t) typedef struct t t;
TYPEDEF(aiScene)
TYPEDEF(aiNode)
TYPEDEF(aiMesh)
TYPEDEF(aiFace)
TYPEDEF(aiMaterial)
TYPEDEF(aiString)
typedef enum aiTextureType aiTextureType;
#undef TYPEDEF

typedef struct {
    Mesh **meshes;
    const char *name;
} Model;

int free_precached_models();

void free_precached_model(Model *);

Model *get_model(const char *, const char *);

Model *get_prop_model(const char *);