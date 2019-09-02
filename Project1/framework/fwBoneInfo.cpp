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
		ret = child->bone(name);
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

/*
 *
 */
void fwBoneInfo::interpolate(time_t start, time_t end, float inbetween_t, glm::mat4* target, glm::mat4 &parent, glm::mat4 &GlobalInverseTransform)
{
	fwAnimationKeyframe* pStart = m_keyframes[start];
	fwAnimationKeyframe* pEnd = m_keyframes[end];

	glm::mat4 globalTransform = parent * pStart->interpolate(pEnd, inbetween_t);
	target[m_id] = GlobalInverseTransform * globalTransform * m_offset;

	for (auto child : m_children) {
		child->interpolate(start, end, inbetween_t, target, globalTransform, GlobalInverseTransform);
	}
}

fwBoneInfo::~fwBoneInfo()
{
	delete m_pWeights;
}