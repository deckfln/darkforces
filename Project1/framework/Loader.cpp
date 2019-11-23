#include "Loader.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <map>

#include "../glEngine/glBufferAttribute.h"

#include "fwTexture.h"
#include "fwGeometry.h"
#include "fwMaterialDiffuse.h"
#include "fwMesh.h"
#include "mesh/fwMeshSkinned.h"
#include "fwAnimation.h"
#include "fwBoneInfoAnimation.h"

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

	// convert temporary meshes to fwMesh
	createFwMesh(scene);

	for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
		fwAnimation *sceneAnimation = processAnimation(scene->mAnimations[i], m_root);

		for (auto mesh : m_meshes) {
			// extract the mesh specific animation from the scene animation
			((fwMeshSkinned*)mesh)->addAnimation(sceneAnimation);
		}
	}
}

fwBoneInfo* Loader::processNode(aiNode *node, fwBoneInfo *parent, const aiScene *scene, int level)
{
	// std::cout << std::string(level, '*') << " " << node->mName.data << " " << node->mNumMeshes << std::endl;

	// process all the node's meshes (if any)
	//if (node->mNumMeshes > 1) { node->mNumMeshes = 1; }
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		//m_meshes.push_back( processMesh_v1(mesh, scene));
		processMesh_v2(mesh, scene);
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

/**
 * Parse a aiMesh to create/update a tempMesh
 *
 **/
void Loader::processMesh_v2(aiMesh* mesh, const aiScene* scene)
{
	tempMesh* tmpMesh = nullptr;
	glm::vec3* position = nullptr;
	glm::vec3* normal = nullptr;
	glm::vec2* uv = nullptr;
	unsigned int* indices = nullptr;
	unsigned int* layer = nullptr;
	int baseVertex = 0;

	/*
	 * lookup for the mesh and extract pointers to the base of the data
	 */
	const std::string name = mesh->mName.data;

	if (m_dMeshes.count(name)) {
		// the mesh already exists => probably a multi-material mesh
		tmpMesh = m_dMeshes[name];
	}
	else {
		m_dMeshes[name] = tmpMesh = new tempMesh;

		tmpMesh->position.resize(128);
		tmpMesh->Normals.resize(128);
		tmpMesh->UVs.resize(128);
		tmpMesh->Indices.resize(128);
		tmpMesh->Layers.resize(128);
		tmpMesh->bonesID.resize(1);
		tmpMesh->bonesWeights.resize(1);
	}

	// build the base
	baseVertex = tmpMesh->nVertices;

	// resize the arrays
	tmpMesh->nVertices += mesh->mNumVertices;

	tmpMesh->position.resize(tmpMesh->nVertices);
	tmpMesh->Normals.resize(tmpMesh->nVertices);
	tmpMesh->UVs.resize(tmpMesh->nVertices);
	tmpMesh->Layers.resize(tmpMesh->nVertices);

	/*
	 *
	 */
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// process vertex m_positions, normals and texture coordinates
		tmpMesh->position[baseVertex + i].x = mesh->mVertices[i].x;
		tmpMesh->position[baseVertex + i].y = mesh->mVertices[i].y;
		tmpMesh->position[baseVertex + i].z = mesh->mVertices[i].z;

		tmpMesh->Normals[baseVertex + i].x = mesh->mNormals[i].x;
		tmpMesh->Normals[baseVertex + i].y = mesh->mNormals[i].y;
		tmpMesh->Normals[baseVertex + i].z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			tmpMesh->UVs[baseVertex + i].x = mesh->mTextureCoords[0][i].x;
			tmpMesh->UVs[baseVertex + i].y = mesh->mTextureCoords[0][i].y;
		}
		else {
			tmpMesh->UVs[baseVertex + i] = glm::vec2(0.0f, 0.0f);
		}

		tmpMesh->Layers[baseVertex + i] = mesh->mMaterialIndex;
	}

	// process indices
	int k = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			k++;

	}

	int baseIndex = tmpMesh->nIndices;
	tmpMesh->nIndices += k;
	tmpMesh->Indices.resize( tmpMesh->nIndices );

	k = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			tmpMesh->Indices[baseIndex  + k] = baseVertex + face.mIndices[j];
			k++;
		}
	}

	if (mesh->mNumBones == 0) {
		return;
	}

	/*
	 * build a skinned mesh
	 */
	int vertices = mesh->mNumVertices;

	tmpMesh->bonesID.resize(tmpMesh->nVertices);
	tmpMesh->bonesWeights.resize(tmpMesh->nVertices);

	unsigned short* nbBonesPerVertices = new unsigned short[vertices]();	// initialize to ZERO : https://stackoverflow.com/questions/2204176/how-to-initialise-memory-with-new-operator-in-c

	fwBoneInfo* bone = nullptr;
	int finalBoneId;	// position of the one in the merged meshes

	for (unsigned int boneID = 0; boneID < mesh->mNumBones; boneID++) {
		aiBone* aib = mesh->mBones[boneID];
		bone = m_bones[aib->mName.data];

		if (bone) {
			finalBoneId = bone->id();

			if (finalBoneId < 0) {
				tmpMesh->boneNames.push_back(aib->mName.data);
				finalBoneId = bone->setIndex(tmpMesh->nBones);
				tmpMesh->nBones++;
			}

			for (unsigned int j = 0; j < aib->mNumWeights; j++) {
				GLint vertexID = aib->mWeights[j].mVertexId;
				float weight = aib->mWeights[j].mWeight;

				unsigned short nbBonesPerVertice = nbBonesPerVertices[vertexID];

				if (nbBonesPerVertice > 3) {
					// std::cout << "Too many bones for vertex " << vertexID << std::endl;
					//refactor the weights by keeping the 4 more importants
					double wmin = weight;
					int index_wmin = -1;
					for (auto i = 0; i < nbBonesPerVertice; i++) {
						if (tmpMesh->bonesWeights[baseVertex + vertexID][i] < wmin) {
							index_wmin = i;
							wmin = tmpMesh->bonesWeights[baseVertex + vertexID][i];
						}
					}

					if (index_wmin > 0) {
						// IF the least important weight is the new one => discard
						// ELSE replace the least important with the new one
						tmpMesh->bonesWeights[baseVertex + vertexID][index_wmin] = weight;
						tmpMesh->bonesID[baseVertex + vertexID][index_wmin] = finalBoneId;
					}

					// and refactor the remaining ones (sum of the existing <=> 1.0)
					float sum = 0;
					for (auto i = 0; i < nbBonesPerVertice; i++) {
						sum += tmpMesh->bonesWeights[baseVertex + vertexID][i];
					}
					for (auto i = 0; i < nbBonesPerVertice; i++) {
						tmpMesh->bonesWeights[baseVertex + vertexID][i] = tmpMesh->bonesWeights[baseVertex + vertexID][i] * 1.0 / sum;
					}
				}
				else {
					// add the weight to the list
					tmpMesh->bonesWeights[baseVertex + vertexID][nbBonesPerVertice] = weight;
					tmpMesh->bonesID[baseVertex + vertexID][nbBonesPerVertice] = finalBoneId;

					nbBonesPerVertices[vertexID]++;
				}

			}

			glm::mat4 offset = aiMatrix4x4ToGlm(aib->mOffsetMatrix);
			bone->offset(offset);

			tmpMesh->lastBone = bone;
		}
		else {
			std::cout << "Loader::processMesh " << aib->mName.data << " unknown" << std::endl;
		}
	}

	delete[] nbBonesPerVertices;
}

/**
 * parse all tempMesh to create fwMesh
 **/
void Loader::createFwMesh(const aiScene *scene)
{
	fwMaterialDiffuse* material = nullptr;

	// for each tmp mesh create a fwMesh of sort
	for (auto const& dMesh : m_dMeshes) {
		tempMesh* tmpMesh = dMesh.second;

		assert(tmpMesh != nullptr);

		fwGeometry* geometry = new fwGeometry();
		int length = tmpMesh->nVertices * sizeof(glm::vec3);
		geometry->addVertices("aPos", &tmpMesh->position[0], 3, length, sizeof(glm::vec3));
		geometry->addAttribute("aNormal", GL_ARRAY_BUFFER, &tmpMesh->Normals[0], 3, length, sizeof(glm::vec3));

		length = tmpMesh->nIndices * sizeof(unsigned int);
		geometry->addIndex(&tmpMesh->Indices[0], 1, length, sizeof(unsigned int));

		length = tmpMesh->nVertices * sizeof(glm::vec2);
		geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, &tmpMesh->UVs[0], 3, length, sizeof(glm::vec2));

		// count the number of materials
		std::map<unsigned int, bool> materials;
		for (auto layer: tmpMesh->Layers) {
			materials[layer] = true;
		}

		// load needed textures
		std::list<fwTexture*> diffuses;
		std::list<fwTexture*> speculars;
		std::list<float> shininesses;

		for (auto material: materials) {
			aiMaterial* aimaterial = scene->mMaterials[material.first];

			aiString name;
			aimaterial->Get(AI_MATKEY_NAME, name);

			fwTexture* diffuse = loadMaterialTextures(aimaterial, aiTextureType_DIFFUSE);
			fwTexture* specular = loadMaterialTextures(aimaterial, aiTextureType_SPECULAR);

			float shininess;
			aimaterial->Get(AI_MATKEY_SHININESS, shininess);

			diffuses.push_back(diffuse);
			shininesses.push_back(shininess);
			if (specular != nullptr) {
				speculars.push_back(specular);
			}
		}

		if (materials.size() == 1) {
			// single texture material

			material = new fwMaterialDiffuse(diffuses.front(), shininesses.front());
			if (speculars.size() > 0) {
				material->specularMap(speculars.front());
			}
		}
		else {
			// multiple textures material

			// add the layer attribute
			// TODO: the layers may be non-sequential : layer 3, layer 5, layer 8 : need to convert to 0,1,2
			length = tmpMesh->Layers.size() * sizeof(float);

			geometry->addAttribute("aLayer", GL_ARRAY_BUFFER, &tmpMesh->Layers[0], 1, length, sizeof(float));

			// merge the list of textures into a texturearray
			fwTextures* diffuse = new fwTextures(diffuses);
			fwTextures* specular = nullptr;
			if (speculars.size() > 0) {
				specular = new fwTextures(speculars);
			}
			material = new fwMaterialDiffuse(diffuse, shininesses.front());
			if (specular != nullptr) {
				material->specularMap(specular);
			}

			// clean up the temporary textures
			for (auto texture : diffuses) {
				delete texture;
			}
			for (auto texture : speculars) {
				delete texture;
			}

		}

		fwMesh* fwmesh;
		if (tmpMesh->nBones == 0) {
			fwmesh = new fwMesh(geometry, material);
			fwmesh->set_name(dMesh.first);
		}
		else {
			fwBoneInfo *rootBone = tmpMesh->lastBone->getRoot(tmpMesh->boneNames);

			geometry->addAttribute("bonesID", GL_ARRAY_BUFFER, &tmpMesh->bonesID[0], 4, tmpMesh->nVertices * sizeof(glm::vec4), sizeof(unsigned int), true);
			geometry->addAttribute("bonesWeight", GL_ARRAY_BUFFER, &tmpMesh->bonesWeights[0], 4, tmpMesh->nVertices * sizeof(glm::vec4), sizeof(float), true);

			glm::mat4 globalInverseTransform = inverse(aiMatrix4x4ToGlm(scene->mRootNode->mTransformation));
			fwmesh = new fwMeshSkinned(geometry, material, rootBone, globalInverseTransform);
		}
		m_meshes.push_back(fwmesh);
	}
}

/**
 * create a texture or textures from the material
 */
fwTexture *Loader::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
	int i = mat->GetTextureCount(type) - 1;
	if (i < 0) {
		return nullptr;
	}

	if (i == 0) {
		aiString str;
		mat->GetTexture(type, 0, &str);

		std::string file = directory + "/" + str.C_Str();

		return new fwTexture(file);
	}

	std::vector<std::string> files;
	files.resize(i + 1);

	for (; i >= 0; i--)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		std::string file = directory + "/" + str.C_Str();

		files[i] = file;
	}
	return (fwTexture *)new fwTextures(i + 1, &files[0]);
}

fwMesh *Loader::get_meshes(int n)
{
	if (n >= 0 && n < m_meshes.size()) {
		return m_meshes[n];
	}

	return nullptr;
}

fwAnimation* Loader::processAnimation(aiAnimation* root, fwBoneInfo* skeleton)
{
	std::map <double, glm::vec3> positions;
	std::map <double, glm::vec4> rotations;

	fwBoneInfoAnimation* boneInfo = nullptr;
	fwAnimationKeyframe* keyframe = nullptr;
	glm::vec3 v3;
	glm::quat quat;
	std::map <time_t, bool> keyframes;

	fwBoneInfoAnimation *animatedSkeleton = new fwBoneInfoAnimation(skeleton);	// clone the skeleton structure
	fwAnimation* animation = new fwAnimation(root->mName.data, root->mDuration * 1000, animatedSkeleton);

	for (unsigned int i = 0; i < root->mNumChannels; i++) {
		aiNodeAnim* node = root->mChannels[i];
		std::string name(node->mNodeName.data);
		time_t time;

		boneInfo = animatedSkeleton->bone(name);
		if (boneInfo == nullptr) {
			std::cout << "cannot find bone " << name << std::endl;
			continue;
		}

		for (unsigned int j = 0; j < node->mNumPositionKeys; j++) {
			aiVectorKey *key = &node->mPositionKeys[j];
			time = key->mTime * 1000;
			keyframe = boneInfo->keyframes(time);

			v3.x = key->mValue.x;
			v3.y = key->mValue.y;
			v3.z = key->mValue.z;
			keyframe->translation(v3);
			keyframes[time] = true;
		}

		for (unsigned int j = 0; j < node->mNumRotationKeys; j++) {
			aiQuatKey* key = &node->mRotationKeys[j];
			time = key->mTime * 1000;
			keyframe = boneInfo->keyframes(time);

			quat.x = key->mValue.x;
			quat.y = key->mValue.y;
			quat.z = key->mValue.z;
			quat.w = key->mValue.w;
			keyframe->rotation(quat);
			keyframes[time] = true;
		}

		for (unsigned int j = 0; j < node->mNumScalingKeys; j++) {
			aiVectorKey* key = &node->mScalingKeys[j];
			time = key->mTime * 1000;
			keyframe = boneInfo->keyframes(time);

			v3.x = key->mValue.x;
			v3.y = key->mValue.y;
			v3.z = key->mValue.z;
			keyframe->scale(v3);
			keyframes[time] = true;
		}
	}

	animation->keyframes(keyframes);
	return animation;
}

Loader::~Loader()
{

}