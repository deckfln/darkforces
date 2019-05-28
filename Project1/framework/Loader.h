#pragma once
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "fwTexture.h"
#include "fwMesh.h"

class Loader 
{
	const std::string file;
	std::string directory;
	std::vector<fwMesh *> meshes;

	void processNode(aiNode *node, const aiScene *scene);
	fwMesh *processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<fwTexture *> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

public:
	Loader(const std::string _file);
	std::vector<fwMesh *>get_meshes(void);
	~Loader();
};
