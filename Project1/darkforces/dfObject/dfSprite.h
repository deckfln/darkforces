#pragma once

#include "../dfObject.h"
#include "../../framework/fwAABBox.h"

class dfFME;
class fwAABBox;
class fwMesh;

class dfSprite : public dfObject
{
public:
	dfSprite(dfFME* fme, const glm::vec3& position, float ambient);
	dfSprite(dfModel* model, const glm::vec3& position, float ambient, int type);
	dfSprite(const std::string& name, const glm::vec3& position, float ambient, int type);

	virtual bool updateSprite(glm::vec3* position, 
		glm::vec4* texture, 
		glm::vec3* direction);
	virtual void OnWorldInsert(void);					// trigger when inserted in a gaWorld
	virtual void OnWorldRemove(void);					// trigger when from the gaWorld
	~dfSprite();
};