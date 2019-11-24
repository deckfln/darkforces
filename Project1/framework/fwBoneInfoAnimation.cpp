#include <iostream>
#include "fwBoneInfoAnimation.h"

fwBoneInfoAnimation::fwBoneInfoAnimation(fwBoneInfo *_bone)
{
	m_bone = _bone;

	fwBoneInfoAnimation* mychild;

	for (auto child: m_bone->get_children()) {
		mychild = new fwBoneInfoAnimation(child);
		m_children.push_back(mychild);
	}
}


/*
 * find the children whose name is name
 */
fwBoneInfoAnimation* fwBoneInfoAnimation::bone(const std::string &name)
{
	if (m_bone->name() == name) {
		return this;
	}

	fwBoneInfoAnimation* ret = nullptr;
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
fwAnimationKeyframe* fwBoneInfoAnimation::keyframes(time_t time)
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
void fwBoneInfoAnimation::interpolate(time_t start, time_t end, float inbetween_t, glm::mat4* target, glm::mat4& parent, glm::mat4& GlobalInverseTransform)
{
	fwAnimationKeyframe* pStart = m_keyframes[start];
	fwAnimationKeyframe* pEnd = m_keyframes[end];
	glm::mat4 globalTransform = parent;

	if (pStart) {
		globalTransform *= pStart->interpolate(pEnd, inbetween_t);
	}

	int id = m_bone->id();
	if (id >= 0) {
		//m_id == -1 => the bone is nor present in the bonesTransform matrix
		assert(id < 64);
		target[id] = GlobalInverseTransform * globalTransform * m_bone->offset();
	}

	for (auto child : m_children) {
		child->interpolate(start, end, inbetween_t, target, globalTransform, GlobalInverseTransform);
	}
}

fwBoneInfoAnimation::~fwBoneInfoAnimation()
{
}