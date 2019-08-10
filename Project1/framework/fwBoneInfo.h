#pragma once

#include <string>
#include <list>
#include <glm/glm.hpp>

class fwBoneInfo
{
	unsigned m_id = -1;
	std::string m_name;
	glm::mat4 m_offset = glm::mat4(0);			// Offset in the skeleton
	glm::mat4 m_transform = glm::mat4(0);		// T-POSE
	float* m_pWeights = nullptr;
	std::list <fwBoneInfo*> m_children;
	fwBoneInfo* m_parent = nullptr;

public:
	fwBoneInfo(std::string name, glm::mat4 transform);
	fwBoneInfo(glm::mat4 offset, float *weights);
	fwBoneInfo* addBone(fwBoneInfo*);
	bool is(std::string name) { return m_name == name; };
	std::list <fwBoneInfo*>& get_children(void) { return m_children; };
	void setIndex(unsigned int index) { m_id = index; };
	void setOffset(glm::mat4 transform) { m_transform = transform; };
	fwBoneInfo* parent(void) { return m_parent; };
	fwBoneInfo* parent(fwBoneInfo* parent) { m_parent = parent; return parent; };
	fwBoneInfo* getRoot(std::list<std::string> &);
	std::string& name(void) { return m_name; };
	~fwBoneInfo();
};