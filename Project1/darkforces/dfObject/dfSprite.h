#pragma once

#include "../dfObject.h"
#include "../../framework/fwAABBox.h"

class dfFME;
class fwAABBox;
class fwMesh;

class dfSprite : public dfObject
{
	glm::vec3 m_position_gl = glm::vec3(0);	// position in gl space
	fwAABBox m_worldBounding;				// bounding box in world gl space
	fwMesh* m_meshAABB = nullptr;			// if we are asked to draw the AABB

public:
	dfSprite(dfFME* fme, glm::vec3& position, float ambient);
	dfSprite(dfModel* model, glm::vec3& position, float ambient, int type);
	fwMesh* drawBoundingBox(void);
	virtual bool updateSprite(glm::vec3* position, glm::vec4* texture, glm::vec3* direction);
	~dfSprite();
};