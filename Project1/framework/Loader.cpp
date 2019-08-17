#include <iostream>

#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <map>

#include "../glEngine/glBufferAttribute.h"

#include "Loader.h"
#include "fwTexture.h"
#include "fwGeometry.h"
#include "fwMaterialDiffuse.h"
#include "fwMesh.h"
#include "mesh/fwMeshSkinned.h"
#include "fwAnimation.h"

static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from)
{
	glm::mat4 to;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

Loader::Loader(const std::string _file):
	file(_file)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = file.substr(0, file.find_last_of('/'));

	fwBoneInfo *m_root = processNode(scene->mRootNode, nullptr, scene, 0);

	for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
		processAnimation(scene->mAnimations[i], m_root);
	}

}

fwBoneInfo* Loader::processNode(aiNode *node, fwBoneInfo *parent, const aiScene *scene, int level)
{
	// std::cout << std::string(level, '*') << " " << node->mName.data << " " << node->mNumMeshes << std::endl;

	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back( processMesh(mesh, scene));
	}

	glm::mat4 transform = aiMatrix4x4ToGlm(node->mTransformation);
	fwBoneInfo *bone = new fwBoneInfo(node->mName.data, transform);
	m_bones[node->mName.data] = bone;

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		fwBoneInfo *child = processNode(node->mChildren[i], bone, scene, level+1);
		bone->addBone(child);
		child->parent(bone);
	}

	return bone;
}

fwMesh *Loader::processMesh(aiMesh *mesh, const aiScene *scene)
{
	/*
	 * build the geometry
	 */
	glm::vec3 *position = new glm::vec3 [mesh->mNumVertices];
	glm::vec3 *normal = new glm::vec3 [mesh->mNumVertices];
	glm::vec2 *uv = new glm::vec2 [mesh->mNumVertices];

	fwGeometry *geometry = new fwGeometry();

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// process vertex m_positions, normals and texture coordinates
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
	unsigned int *indice = new unsigned int [k];

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
	fwMaterialDiffuse *material = nullptr;
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *aimaterial = scene->mMaterials[mesh->mMaterialIndex];

		aiString name;
		aimaterial->Get(AI_MATKEY_NAME, name);

		std::vector<fwTexture *> diffuse = loadMaterialTextures(aimaterial, aiTextureType_DIFFUSE);
		std::vector<fwTexture *> specular = loadMaterialTextures(aimaterial, aiTextureType_SPECULAR);

		float shininess;
		aimaterial->Get(AI_MATKEY_SHININESS, shininess);

		material = new fwMaterialDiffuse(diffuse[0], shininess);
		if (specular.size() > 0) {
			material->specularMap(specular[0]);
		}
	}

	if (mesh->mNumBones == 0) {
		fwMesh* fwmesh;
		fwmesh = new fwMesh(geometry, material);
		return fwmesh;
	}

	/*
	 * build a skinned mesh
	 */
	int vertices = mesh->mNumVertices;

	glm::ivec4* bonesID = new glm::ivec4[vertices]();	// init with 0
	glm::vec4* bonesWeights = new glm::vec4[vertices](); // init with 0
	unsigned short* nbBonesPerVertices = new unsigned short[vertices]();	// initialize to ZERO : https://stackoverflow.com/questions/2204176/how-to-initialise-memory-with-new-operator-in-c

	std::list <std::string> boneNames;

	fwBoneInfo* bone = nullptr;

	for (unsigned int boneID = 0; boneID < mesh->mNumBones; boneID++) {
		aiBone* aib = mesh->mBones[boneID];
		bone = m_bones[aib->mName.data];

		if (bone) {
			boneNames.push_back(aib->mName.data);
			bone->setIndex(boneID);

			for (unsigned int j = 0; j < aib->mNumWeights; j++) {
				GLint vertexID = aib->mWeights[j].mVertexId;
				float weight = aib->mWeights[j].mWeight;

				unsigned short nbBonesPerVertice = nbBonesPerVertices[vertexID];

				if (nbBonesPerVertice > 3) {
					// std::cout << "Too many bones for vertex " << vertexID << std::endl;
					//refactor the weights by keeping the 4 more importants
					float wmin = weight;
					int index_wmin = -1;
					for (auto i = 0; i < nbBonesPerVertice; i++) {
						if (bonesWeights[vertexID][i] < wmin) {
							index_wmin = i;
							wmin = bonesWeights[vertexID][i];
						}
					}

					if (index_wmin > 0) {
						// IF the least important weight is the new one => discard
						// ELSE replace the least important with the new one
						bonesWeights[vertexID][index_wmin] = weight;
						bonesID[vertexID][index_wmin] = boneID;
					}

					// and refactor the remaining ones (sum of the existing <=> 1.0)
					float sum = 0;
					for (auto i = 0; i < nbBonesPerVertice; i++) {
						sum += bonesWeights[vertexID][i];
					}
					for (auto i = 0; i < nbBonesPerVertice; i++) {
						bonesWeights[vertexID][i] = bonesWeights[vertexID][i] * 1.0 / sum;
					}
				}
				else {
					// add the weight to the list
					bonesWeights[vertexID][nbBonesPerVertice] = weight;
					bonesID[vertexID][nbBonesPerVertice] = boneID;

					nbBonesPerVertices[vertexID]++;
				}

			}

			glm::mat4 offset = aiMatrix4x4ToGlm(aib->mOffsetMatrix);
			bone->offset(offset);
		}
		else {
			std::cout << "Loader::processMesh " << aib->mName.data << " unknown" << std::endl;
		}
	}

	// find root of the skeleton: the upper name that is also present in the list of bones
	fwBoneInfo *root = bone->getRoot(boneNames);

	geometry->addAttribute("bonesID", GL_ARRAY_BUFFER, bonesID, 4, vertices * sizeof(glm::vec4), sizeof(unsigned int), true);
	geometry->addAttribute("bonesWeight", GL_ARRAY_BUFFER, bonesWeights, 4, vertices * sizeof(glm::vec4), sizeof(float), true);

	glm::mat4 globalInverseTransform = inverse(aiMatrix4x4ToGlm(scene->mRootNode->mTransformation));
	fwMeshSkinned* fwmesh = new fwMeshSkinned(geometry, material, root, globalInverseTransform);

	return fwmesh;
}

std::vector<fwTexture *> Loader::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
	std::vector<fwTexture *> textures;

	for (int i = mat->GetTextureCount(type) - 1; i >= 0; i--)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		std::string file = directory + "/" + str.C_Str();

		textures.push_back(new fwTexture(file));
	}

	return textures;
}

std::vector<fwMesh *>Loader::get_meshes(void)
{
	return meshes;
}

fwAnimation* Loader::processAnimation(aiAnimation* root, fwBoneInfo* skeleton)
{
	fwAnimation* animation = new fwAnimation(root->mName.data, root->mDuration, skeleton);

	std::map <double, glm::vec3> positions;
	std::map <double, glm::vec4> rotations;

	for (unsigned int i = 0; i < root->mNumChannels; i++) {
		aiNodeAnim* node = root->mChannels[i];
		std::string name(node->mNodeName.data);

		for (unsigned int j = 0; j < node->mNumPositionKeys; j++) {
			aiVectorKey *key = &node->mPositionKeys[j];
			double time = key->mTime;
			fwAnimationKeyframe* keyframe = animation->keyframes(time);
			fwAnimationBone* boneInfo = keyframe->bone(name);

			if (boneInfo == nullptr) {
				std::cout << "cannot find bone " << name << std::endl;
				continue;
			}
			boneInfo->translation(glm::vec3(key->mValue.x, key->mValue.y, key->mValue.z));
		}

		for (unsigned int j = 0; j < node->mNumRotationKeys; j++) {
			aiQuatKey* key = &node->mRotationKeys[j];
			double time = key->mTime;
			fwAnimationKeyframe* keyframe = animation->keyframes(time);
			fwAnimationBone* boneInfo = keyframe->bone(name);

			if (boneInfo == nullptr) {
				std::cout << "cannot find bone " << name << std::endl;
				continue;
			}
			boneInfo->rotation(glm::vec4(key->mValue.x, key->mValue.y, key->mValue.z, key->mValue.w));
		}

		for (unsigned int j = 0; j < node->mNumScalingKeys; j++) {
			aiVectorKey* key = &node->mScalingKeys[j];
			double time = key->mTime;
			fwAnimationKeyframe* keyframe = animation->keyframes(time);
			fwAnimationBone* boneInfo = keyframe->bone(name);

			if (boneInfo == nullptr) {
				std::cout << "cannot find bone " << name << std::endl;
				continue;
			}
			boneInfo->scale(glm::vec3(key->mValue.x, key->mValue.y, key->mValue.z));
		}
	}

	return animation;
}

Loader::~Loader()
{

}