#include "fwAnimationKeyframe.h"

#include <iostream>

fwAnimationKeyframe::fwAnimationKeyframe(double time, fwBoneInfo* skeleton) :
	m_time(time)
{
	m_skeleton = cloneSkeleton(skeleton);
}

fwAnimationBone* fwAnimationKeyframe::cloneSkeleton(fwBoneInfo* boneinfo, int level)
{
	fwAnimationBone* bone = new fwAnimationBone(boneinfo);
	
	for (auto child : boneinfo->get_children()) {
		bone->addChild(cloneSkeleton(child, level + 1));
	}

	return bone;
}

fwAnimationBone* fwAnimationKeyframe::bone(std::string name)
{
	return m_skeleton->find(name);
}

fwAnimationKeyframe::~fwAnimationKeyframe()
{

}
