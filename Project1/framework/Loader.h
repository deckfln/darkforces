#pragma once
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Texture.h"

class Loader 
{
	const std::string file;
	std::string directory;

	void processNode(aiNode *node, const aiScene *scene);
	void processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture *> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

public:
	Loader(const std::string _file);
	~Loader();
};
