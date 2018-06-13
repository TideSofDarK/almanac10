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
	const char * name;
} Model;

void construct_model(Model**, const char*, const char *);
void destruct_model(Model**);

void construct_prop(Model **, const char*);