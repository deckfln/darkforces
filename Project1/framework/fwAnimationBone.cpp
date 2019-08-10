#include "fwAnimationBone.h"

fwAnimationBone::fwAnimationBone(fwBoneInfo* boneinfo)
{
	m_bone = boneinfo;
}

void fwAnimationBone::addChild(fwAnimationBone* bone)
{
	children.push_back(bone);
}

fwAnimationBone* fwAnimationBone::find(std::string name)
{
	if (m_bone->is(name)) {
		return this;
	}

	fwAnimationBone* found = nullptr;

	for (auto child : children) {
		found = child->find(name);
		if (found) {
			break;
		}
	}

	return found;
}

fwAnimationBone::~fwAnimationBone()
{

}