#include "model.h"

#include "util.h"

void process_mesh(const aiScene* ai_scene, aiMesh* ai_mesh, Model* model, Mesh** _mesh)
{
	/* Vertices, normals, texcoords, indices */
	Vertex* vertices = NULL;
	int* indices = NULL;
	for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
	{
		Vertex vertex;

		glm_vec_copy((vec3) { ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z }, vertex.pos);

		if (ai_mesh->mNormals && ai_mesh->mNormals[i].x)
		{
			glm_vec_copy((vec3) { ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z }, vertex.normal);
		}
		else
		{
			glm_vec_zero(vertex.normal);
		}

		if (ai_mesh->mTextureCoords[0])
		{
			glm_vec_copy((vec2) { ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y }, vertex.tex_coords);
		}
		else
		{
			glm_vec_copy((vec2) { 0.0f, 0.0f }, vertex.tex_coords);
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

	/* Textures; check if already loaded */
	aiString str;
	if (ai_mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
		aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &str, NULL, NULL, NULL, NULL, NULL, NULL);
	}

	char * filename = _strdup(str.data);

	Texture* texture = NULL;
	cached_texture(filename, &texture);
	construct_mesh(vertices, indices, texture, _mesh);

	free(filename);
}

void process_node(Model* model, aiNode *node, const aiScene *scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		Mesh* mesh = NULL;
		process_mesh(scene, scene->mMeshes[node->mMeshes[i]], model, &mesh);
		vector_push_back(model->meshes, mesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		process_node(model, node->mChildren[i], scene);
	}
}

void construct_model(const char* filename, Model** _model)
{
	*_model = malloc(sizeof(Model));
	Model *model = *_model;

	model->meshes = NULL;

	const aiScene* ai_scene = aiImportFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
	process_node(*_model, ai_scene->mRootNode, ai_scene);

	aiReleaseImport(ai_scene);
}

void destruct_model(Model** _model)
{
	Model *model = *_model;

	for (unsigned int i = 0; i < vector_size(model->meshes); i++)
		destruct_mesh(&model->meshes[i]);
	vector_free(model->meshes);
	model->meshes = NULL;

	free(*_model);
	*_model = NULL;
}