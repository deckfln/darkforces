#pragma once

#include <string>
#include "fwBoneInfo.h"

class fwAnimationBone {
	fwBoneInfo* m_bone;

	std::list <fwAnimationBone*> children;
	glm::vec3 m_translation = glm::vec3(0);
	glm::vec4 m_rotation= glm::vec4(0);
	glm::vec3 m_scale = glm::vec3(0);


public:
	fwAnimationBone(fwBoneInfo* boneinfo);
	void addChild(fwAnimationBone* boneinfo);
	fwAnimationBone* find(std::string name);
	void translation(glm::vec3 t) { m_translation = t; };
	void rotation(glm::vec4 r) { m_rotation = r; };
	void scale(glm::vec3 s) { m_scale = s; };
	~fwAnimationBone();
};