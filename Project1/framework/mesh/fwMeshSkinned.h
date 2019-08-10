#pragma once

#include <map>

#include "../fwMesh.h"
#include "..//fwBoneInfo.h"
#include "..//fwAnimation.h"

class fwMeshSkinned : public fwMesh
{
	glm::mat4* m_bonesOffset = nullptr;	// local transform of each bone : per bone
	glm::ivec4* m_bonesID = nullptr;		// index bones affecting the vertex : per vertex, maximum 3 bones per vertex
	glm::vec4* m_bonesWeights = nullptr;	// wdith of the bones affecting the vertex : per vertex, maximum 3 bones per vertex
	std::map <const std::string, unsigned int> m_bonesIndex;
	fwBoneInfo* m_skeleton = nullptr;
	std::map <const std::string, fwAnimation*> m_animations;

public:
	fwMeshSkinned(fwGeometry* _geometry, fwMaterial* _material);
	void bonesIndex(std::map <const std::string, unsigned int> &bonesIndex);
	void bonesID(glm::ivec4 *);
	void bonesWeights(glm::vec4 *);
	void addAnimation(const std::string name, fwAnimation* animation);
	fwBoneInfo* skeleton(void) { return m_skeleton; };
	fwBoneInfo* skeleton(fwBoneInfo* skeleton) { m_skeleton = skeleton;  return m_skeleton; };
	~fwMeshSkinned();
};