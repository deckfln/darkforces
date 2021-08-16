#pragma once

#include "../dfObject.h"
#include "../../framework/fwAABBox.h"
#include "../dfComponent/dfComponentLogic.h"
#include "../flightRecorder/frSprite.h"

class dfFME;
class fwAABBox;
class fwMesh;

class dfSprite : public dfObject
{
	dfComponentLogic m_componentLogic;		// dealing with the logic of the object

public:
	dfSprite(dfFME* fme, const glm::vec3& position, float ambient, uint32_t objectID);
	dfSprite(dfModel* model, const glm::vec3& position, float ambient, int type, uint32_t objectID);
	dfSprite(const std::string& name, const glm::vec3& position, float ambient, int type);
	dfSprite(flightRecorder::DarkForces::Sprite* record);

	virtual bool updateSprite(glm::vec3* position, 
		glm::vec4* texture, 
		glm::vec3* direction);
	void OnWorldInsert(void) override;					// trigger when inserted in a gaWorld
	void OnWorldRemove(void) override;					// trigger when from the gaWorld

	// flight recorder and debugger
	inline int recordSize(void) override {
		return sizeof(flightRecorder::DarkForces::Sprite);
	}													// size of one record
	uint32_t recordState(void* record) override;			// return a record of the entity state (for debug)
	void loadState(flightRecorder::Entity* record) override;// reload an entity state from a record

	~dfSprite();
};