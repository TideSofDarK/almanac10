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

typedef struct
{
	Mesh** meshes;
} Model;

void construct_model(const char*, Model**);
void destruct_model(Model**);