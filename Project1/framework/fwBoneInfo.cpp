#include "fwBoneInfo.h"

#include <algorithm>

fwBoneInfo::fwBoneInfo(std::string name, glm::mat4 &transform):
	m_name(name),
	m_transform(transform)
{
}

fwBoneInfo::fwBoneInfo(glm::mat4 offset, float* weights)
{
	m_offset = offset;
	m_pWeights = weights;
}

fwBoneInfo *fwBoneInfo::addBone(fwBoneInfo* bone)
{
	m_children.push_back(bone);

	return this;
}

fwBoneInfo* fwBoneInfo::getRoot(std::list<std::string> &names)
{
	fwBoneInfo* current = this;
	fwBoneInfo* parent = nullptr;

	for (; current->m_parent != nullptr; current = parent) {
		parent = current->m_parent;

		if (std::find(names.begin(), names.end(), parent->m_name) == names.end()) {
			return current;
		}
	}

	return nullptr;
}

/*
 * find the children whose name is name
 */
fwBoneInfo* fwBoneInfo::bone(const std::string name)
{
	if (m_name == name) {
		return this;
	}

	fwBoneInfo* ret = nullptr;
	for (auto child : m_children) {
		ret = bone(name);
		if (ret) {
			return ret;
		}
	}

	return nullptr;
}

/*
 * find or allocate a keyframe
 */
fwAnimationKeyframe* fwBoneInfo::keyframes(time_t time)
{
	fwAnimationKeyframe* keyframe = m_keyframes[time];
	if (keyframe == nullptr) {
		keyframe = m_keyframes[time] = new fwAnimationKeyframe(time);
	}

	return keyframe;
}

fwBoneInfo::~fwBoneInfo()
{
	delete m_pWeights;
}