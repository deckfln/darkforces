#pragma once

#include <map>

#include "../fwMesh.h"
#include "../fwBoneInfo.h"
#include "../fwAnimation.h"

constexpr auto SKINNED_MESH = 2;

class fwMeshSkinned : public fwMesh
{
	glm::mat4* m_bonesOffset = nullptr;	// local transform of each bone : per bone
	glm::ivec4* m_bonesID = nullptr;		// index bones affecting the vertex : per vertex, maximum 3 bones per vertex
	glm::vec4* m_bonesWeights = nullptr;	// wdith of the bones affecting the vertex : per vertex, maximum 3 bones per vertex
	std::map <const std::string, unsigned int> m_bonesIndex;
	fwBoneInfo* m_skeleton = nullptr;
	glm::mat4 *m_bonesTransform = nullptr;
	glm::mat4 m_GlobalInverseTransform = glm::mat4(1.0);

	fwAnimation* m_currentAnimation = nullptr;
	std::map <const std::string, fwAnimation*> m_animations;

	void t_pose(fwBoneInfo* bone, glm::mat4& parent);

public:
	fwMeshSkinned(fwGeometry* _geometry, fwMaterial* _material, fwBoneInfo *root, glm::mat4 &globalInverseTransform);
	void bonesIndex(std::map <const std::string, unsigned int> &bonesIndex);
	void bonesID(glm::ivec4 *, int nb);
	void bonesWeights(glm::vec4 *, int nb);
	void addAnimation(fwAnimation* animation);

	fwBoneInfo* skeleton(void) { return m_skeleton; };
	fwBoneInfo* skeleton(fwBoneInfo* skeleton) { m_skeleton = skeleton;  return m_skeleton; };

	void run(const std::string animation);
	void stop(void);
	void update(time_t delta);

	~fwMeshSkinned();
};