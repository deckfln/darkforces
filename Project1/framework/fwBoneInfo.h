#pragma once

#include <string>
#include <map>
#include <list>
#include <glm/glm.hpp>

class fwBoneInfo
{
	int m_id = -1;		// Position of the bone in the bonesTraform matrix: -1 => not present
	std::string m_name;
	glm::mat4 m_offset = glm::mat4(1);			// Offset in the skeleton
	glm::mat4 m_transform = glm::mat4(1);		// T-POSE
	float* m_pWeights = nullptr;
	std::list <fwBoneInfo*> m_children;
	fwBoneInfo* m_parent = nullptr;

public:
	fwBoneInfo(std::string name, glm::mat4 &transform);
	fwBoneInfo(glm::mat4 offset, float *weights);
	fwBoneInfo* addBone(fwBoneInfo*);
	bool is(std::string name) { return m_name == name; };
	std::list <fwBoneInfo*>& get_children(void) { return m_children; };
	int setIndex(int index) {
		if (m_id < 0) {
			m_id = index;
		}
		return m_id;
	}

	void offset(glm::mat4 &offset) { m_offset = offset; };
	glm::mat4& offset(void) { return m_offset; };

	void transform(glm::mat4& transform) { m_transform = transform; };
	glm::mat4& transform(void) { return m_transform; };

	fwBoneInfo* parent(void) { return m_parent; };
	fwBoneInfo* parent(fwBoneInfo* parent) { m_parent = parent; return parent; };

	std::string& name(void) { return m_name; };

	fwBoneInfo* bone(const std::string name);

	fwBoneInfo* getRoot(std::list<std::string> &);
	unsigned id(void) { return m_id; };
	~fwBoneInfo();
};