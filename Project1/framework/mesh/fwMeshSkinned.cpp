#include "fwMeshSkinned.h"

fwMeshSkinned::fwMeshSkinned(fwGeometry* _geometry, fwMaterial* _material, fwBoneInfo *root, glm::mat4& globalInverseTransform):
	fwMesh(_geometry, _material),
	m_skeleton(root),
	m_GlobalInverseTransform(globalInverseTransform)
{
	classID |= SKINNED_MESH;
	m_bonesTransform = new glm::mat4[64]();

	stop();
	addUniform(new fwUniform("gBones[0]", m_bonesTransform, 64));
}

void fwMeshSkinned::bonesID(glm::ivec4* bonesID, int nb)
{
	m_bonesID = bonesID;
}

void fwMeshSkinned::bonesWeights(glm::vec4* bonesWeights, int nb)
{
	m_bonesWeights = bonesWeights;
}

void fwMeshSkinned::bonesIndex(std::map <const std::string, unsigned int>& bonesIndex)
{
	m_bonesIndex = bonesIndex;
}

void fwMeshSkinned::addAnimation(fwAnimation* animation)
{
	// find each keyframe starting at root bone
	m_animations[animation->name()] = animation->extract(m_skeleton);
}

/*
 * complete the bonesTransforms with the t-pose
 */
void fwMeshSkinned::t_pose(fwBoneInfo* bone, glm::mat4& parent)
{
	int id = bone->id();
	// std::cout << bone->name() << " " << id << std::endl;
	glm::mat4 globalTransform = parent * bone->transform();
	if (id >= 0) {
		m_bonesTransform[id] = m_GlobalInverseTransform * globalTransform * bone->offset();
	}

	std::list <fwBoneInfo*>children = bone->get_children();
	for (auto child : children) {
		t_pose(child, globalTransform);
	}
}

/*
 * start an animation
 */
void fwMeshSkinned::run(const std::string animation)
{
	m_currentAnimation = m_animations[animation];
	m_currentAnimation->reset();
}

/*
 * move to t-pose
 */
void fwMeshSkinned::stop(void)
{
	glm::mat4 identity(1);

	m_currentAnimation = nullptr;
	t_pose(m_skeleton, identity);
}

/*
 * run the current animation
 */
void fwMeshSkinned::update(void)
{
	if (m_currentAnimation == nullptr) {
		// no running animation
		return;
	}

	m_currentAnimation->update(m_bonesTransform);
}

fwMeshSkinned::~fwMeshSkinned()
{
	delete m_bonesOffset;
	delete m_bonesID;
	delete m_bonesWeights;
	delete m_bonesTransform;
	
	for (auto animation : m_animations) {
		delete animation.second;
	}
}