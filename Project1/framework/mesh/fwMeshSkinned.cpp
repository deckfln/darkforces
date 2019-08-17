#include "fwMeshSkinned.h"

fwMeshSkinned::fwMeshSkinned(fwGeometry* _geometry, fwMaterial* _material, fwBoneInfo *root, glm::mat4& globalInverseTransform):
	fwMesh(_geometry, _material),
	m_skeleton(root),
	m_GlobalInverseTransform(globalInverseTransform)
{
	classID |= SKINNED_MESH;
	m_bonesTransform = new glm::mat4[64]();

	glm::mat4 identity(1);
	animate(root, identity);
	addUniform(new fwUniform("gBones[0]", m_bonesTransform, 64));
}

void fwMeshSkinned::animate(fwBoneInfo *bone, glm::mat4 &parent)
{
	int id = bone->id();
	std::cout << bone->name() << " " << id << std::endl;
	glm::mat4 globalTransform = parent * bone->transform();
	if (id >= 0) {
		m_bonesTransform[id] = m_GlobalInverseTransform * globalTransform * bone->offset();
	}

	std::list <fwBoneInfo*>children = bone->get_children();
	for (auto child : children) {
		animate(child, globalTransform);
	}
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

void fwMeshSkinned::addAnimation(const std::string name, fwAnimation* animation)
{
	m_animations[name] = animation;
}

fwMeshSkinned::~fwMeshSkinned()
{
	delete m_bonesOffset;
	delete m_bonesID;
	delete m_bonesWeights;
	delete m_bonesTransform;
}