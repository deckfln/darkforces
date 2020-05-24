#pragma once

#include "../dfObject.h"

class dfFME;

class dfSprite : public dfObject
{
public:
	dfSprite(dfFME* fme, glm::vec3& position, float ambient);
	dfSprite(dfModel* model, glm::vec3& position, float ambient, int type);
	virtual bool updateSprite(glm::vec3* position, glm::vec4* texture, glm::vec3* direction);
	~dfSprite();
};