#pragma once

#include "../dfObject.h"
#include "../dfComponent/dfComponentLogic.h"
#include "../../gaEngine/gaComponent/gaComponentMesh.h"

#include "../flightRecorder/Object3D.h"

class fwMesh;
class fwScene;

class gaMessage;

class df3DO;
class dfVue;
class dfFileSystem;

class dfObject3D : public DarkForces::Object
{
	dfComponentLogic m_componentLogic;
	GameEngine::ComponentMesh m_componentMesh;

	glm::vec3 m_animRotationAxe = glm::vec3(0);
	float m_aniRotationSpeed = 0;	//rotates from -999 (max anti-clockwise) to 999 (max clockwise)

	dfVue* m_vue = nullptr;

	bool m_pause = false;			// pause at the end of the animation

	time_t m_lastFrame = 0;			// timestamps of the last frame

public:
	dfObject3D(df3DO* threedo, const glm::vec3& position, float ambient, uint32_t objectID);
	dfObject3D(flightRecorder::DarkForces::Object3D*);
	void animRotationAxe(int axe);
	void animRotationSpeed(float s);
	void pause(bool p) { m_pause = p; };
	void vue(dfFileSystem *fs, const std::string& vue, const std::string& component);

	bool update(time_t t) override;						// update based on timer
	// virtual void updateWorldAABB(void);
	void dispatchMessage(gaMessage* message) override;

	// flight recorder and debugger
	inline int recordSize(void) override {
		return sizeof(flightRecorder::DarkForces::Object3D);
	}													// size of one record
	uint32_t recordState(void* record) override;			// return a record of the entity state (for debug)
	void loadState(void* record) override;// reload an entity state from a record
	void debugGUIChildClass(void) override;				// Add dedicated component debug the entity

	~dfObject3D();
};