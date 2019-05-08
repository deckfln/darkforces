#pragma once
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Texture.h"
#include "Mesh.h"

class Loader 
{
	const std::string file;
	std::string directory;
	std::vector<Mesh *> meshes;

	void processNode(aiNode *node, const aiScene *scene);
	Mesh *processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture *> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

public:
	Loader(const std::string _file);
	std::vector<Mesh *>get_meshes(void);
	~Loader();
};
