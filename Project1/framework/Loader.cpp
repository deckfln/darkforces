#include <iostream>

#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include "glEngine/glBufferAttribute.h"

#include "Loader.h"
#include "Texture.h"
#include "fwGeometry.h"
#include "fwDiffuseMaterial.h"
#include "fwMesh.h"

Loader::Loader(const std::string _file):
	file(_file)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = file.substr(0, file.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Loader::processNode(aiNode *node, const aiScene *scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back( processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

fwMesh *Loader::processMesh(aiMesh *mesh, const aiScene *scene)
{
	/*
	 * build the geometry
	 */
	glm::vec3 *position = (glm::vec3 *)calloc(mesh->mNumVertices, sizeof(glm::vec3));
	glm::vec3 *normal = (glm::vec3 *)calloc(mesh->mNumVertices, sizeof(glm::vec3));
	glm::vec2 *uv = (glm::vec2 *)calloc(mesh->mNumVertices, sizeof(glm::vec2));

	fwGeometry *geometry = new fwGeometry();

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// process vertex positions, normals and texture coordinates
		position[i].x = mesh->mVertices[i].x;
		position[i].y = mesh->mVertices[i].y;
		position[i].z = mesh->mVertices[i].z;

		normal[i].x = mesh->mNormals[i].x;
		normal[i].y = mesh->mNormals[i].y;
		normal[i].z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			uv[i].x = mesh->mTextureCoords[0][i].x;
			uv[i].y = mesh->mTextureCoords[0][i].y;
		}
		else
			uv[i] = glm::vec2(0.0f, 0.0f);
	}

	int length = mesh->mNumVertices * sizeof(glm::vec3);
	geometry->addVertices("aPos", position, 3, length, sizeof(glm::vec3));
	geometry->addAttribute("aNormal", GL_ARRAY_BUFFER, normal, 3, length, sizeof(glm::vec3));

	length = mesh->mNumVertices * sizeof(glm::vec2);
	geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, uv, 3, length, sizeof(glm::vec2));

	// process indices
	int k = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			k++;

	} 
	unsigned int *indice = (unsigned int *)calloc(k, sizeof(unsigned int));

	k = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indice[k] = face.mIndices[j];
			k++;
		}
	}
	geometry->addIndex(indice, 1, k * sizeof(unsigned int), sizeof(unsigned int));

	/*
	 * build the material
	 */
	fwDiffuseMaterial *material = nullptr;
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *aimaterial = scene->mMaterials[mesh->mMaterialIndex];

		aiString name;
		aimaterial->Get(AI_MATKEY_NAME, name);

		std::vector<Texture *> diffuse = loadMaterialTextures(aimaterial, aiTextureType_DIFFUSE);
		std::vector<Texture *> specular = loadMaterialTextures(aimaterial, aiTextureType_SPECULAR);

		float shininess;
		aimaterial->Get(AI_MATKEY_SHININESS, shininess);

		if (specular.size() > 0) {
			material = new fwDiffuseMaterial(diffuse[0], specular[0], shininess);
		}
		else {
			material = new fwDiffuseMaterial(diffuse[0], nullptr, shininess);
		}
	}

	fwMesh *nmesh = new fwMesh(geometry, material);

	return nmesh;
}


std::vector<Texture *> Loader::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
	std::vector<Texture *> textures;

	for (int i = mat->GetTextureCount(type) - 1; i >= 0; i--)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		std::string file = directory + "/" + str.C_Str();

		textures.push_back(new Texture(file));
	}

	return textures;
}

std::vector<fwMesh *>Loader::get_meshes(void)
{
	return meshes;
}

Loader::~Loader()
{

}