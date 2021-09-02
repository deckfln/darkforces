#include "fwMeshSkinned.h"

#include "../fwUniform.h"

fwMeshSkinned::fwMeshSkinned(fwGeometry* _geometry, fwMaterial* _material, fwBoneInfo *root, glm::mat4& globalInverseTransform):
	fwMesh(_geometry, _material),
	m_skeleton(root),
	m_GlobalInverseTransform(globalInverseTransform)
{
	m_classID |= Framework::ClassID::SKINNED_MESH;
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

void fwMeshSkinned::addAnimation(fwAnimation* sceneAnimation)
{
	// find the mesh in the scene animation
	m_animations[sceneAnimation->name()] = sceneAnimation;
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
		// id == -1 => the bone is not present in the bonesTrasform matrix
		assert(id < 64);
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
	if (m_currentAnimation == nullptr) {
		std::cout << "fwMeshSkinned::run animation *" << animation << "* missing" << std::endl;
		exit(-1);
	}
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
void fwMeshSkinned::update(time_t delta)
{
	if (m_currentAnimation == nullptr) {
		// no running animation
		return;
	}

	m_currentAnimation->update(delta, m_bonesTransform, m_GlobalInverseTransform);
}

fwMeshSkinned::~fwMeshSkinned()
{
	for (auto animation : m_animations) {
		delete animation.second;
	}

	delete[] m_bonesTransform;

	delete m_skeleton;
}