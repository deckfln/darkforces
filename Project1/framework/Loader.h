#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <assimp/scene.h>

#include "fwTexture.h"
#include "fwMesh.h"
#include "fwBoneInfo.h"
#include "fwAnimation.h"

struct tempMesh
{
	int m_baseIndex = 0;

	int nVertices = 0;

	// per vertex
	std::vector <glm::vec3> position;
	std::vector<glm::vec3> Normals;
	std::vector <glm::vec2> UVs;
	std::vector<unsigned int> Layers;
	std::vector<glm::ivec4> bonesID;
	std::vector<glm::vec4> bonesWeights;

	int nIndices = 0;
	std::vector<unsigned int> Indices;

	int nBones = 0;
	fwBoneInfo* lastBone = nullptr;	// last bone loaded on the mesh
	std::list <std::string> boneNames;	// list of bones on the mesh
};

class Loader 
{
	const std::string file;
	std::string directory;
	std::map<std::string, tempMesh *>m_dMeshes;
	std::vector<fwMesh*>m_meshes;
	fwBoneInfo* m_root = nullptr;
	std::map <std::string, fwBoneInfo*>m_bones;

	fwBoneInfo* processNode(aiNode* node, fwBoneInfo* bone, const aiScene* scene, int level);
	fwAnimation* processAnimation(aiAnimation*, fwBoneInfo* skeleton);
	fwMesh *processMesh_v1(aiMesh *mesh, const aiScene *scene);
	void processMesh_v2(aiMesh* mesh, const aiScene* scene);
	void createFwMesh(const aiScene* scene);
	fwTexture * loadMaterialTextures(aiMaterial *mat, aiTextureType type);

public:
	Loader(const std::string _file);
	fwMesh *get_meshes(int n);
	~Loader();
};
