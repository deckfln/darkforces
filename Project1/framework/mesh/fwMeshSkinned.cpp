#include "fwMeshSkinned.h"

fwMeshSkinned::fwMeshSkinned(fwGeometry* _geometry, fwMaterial* _material):
	fwMesh(_geometry, _material)
{

}

void fwMeshSkinned::bonesID(glm::ivec4* bonesID)
{
	m_bonesID = bonesID;
}

void fwMeshSkinned::bonesWeights(glm::vec4* bonesWeights)
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
}