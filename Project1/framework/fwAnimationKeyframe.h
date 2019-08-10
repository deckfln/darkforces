#pragma once

#include <glm/glm.hpp>
#include <list>
#include "fwAnimationBone.h"
#include "fwBoneInfo.h"

class fwAnimationKeyframe
{
	double m_time;
	fwAnimationBone* m_skeleton;
	fwAnimationBone* cloneSkeleton(fwBoneInfo* skeleton, int level=0);

public:
	fwAnimationKeyframe(double time, fwBoneInfo* skeleton);
	void addChild(fwAnimationBone* boneinfo);
	fwAnimationBone* bone(std::string name);
	~fwAnimationKeyframe();
};
