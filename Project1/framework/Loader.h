#pragma once
#include <iostream>
#include <vector>

#include <assimp/scene.h>

#include "fwTexture.h"
#include "fwMesh.h"
#include "fwBoneInfo.h"
#include "fwAnimation.h"

class Loader 
{
	const std::string file;
	std::string directory;
	std::vector<fwMesh *> meshes;
	fwBoneInfo* m_root = nullptr;
	std::map <std::string, fwBoneInfo*>m_bones;

	fwBoneInfo* processNode(aiNode* node, fwBoneInfo* bone, const aiScene* scene, int level);
	fwAnimation* processAnimation(aiAnimation*, fwBoneInfo* skeleton);
	fwMesh *processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<fwTexture *> loadMaterialTextures(aiMaterial *mat, aiTextureType type);

public:
	Loader(const std::string _file);
	std::vector<fwMesh *>get_meshes(void);
	~Loader();
};
