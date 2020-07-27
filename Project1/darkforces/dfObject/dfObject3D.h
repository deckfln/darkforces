#pragma once

#include "../dfObject.h"
#include "../dfComponent/dfComponentLogic.h"
#include "../../gaEngine/gaComponent/gaComponentMesh.h"

class fwMesh;
class fwScene;

class gaMessage;

class df3DO;
class dfVue;
class dfFileSystem;

class dfObject3D : public dfObject
{
	dfComponentLogic m_componentLogic;
	GameEngine::ComponentMesh m_componentMesh;

	glm::vec3 m_animRotation = glm::vec3(0);
	glm::vec3 m_animRotationAxe = glm::vec3(0);
	float m_aniRotationSpeed = 0;	//rotates from -999 (max anti-clockwise) to 999 (max clockwise)

	dfVue* m_vue = nullptr;

	bool m_pause = false;			// pause at the end of the animation

	time_t m_lastFrame = 0;			// timestamps of the last frame

public:
	dfObject3D(df3DO* threedo, glm::vec3& position, float ambient);
	void animRotationAxe(int axe);
	void animRotationSpeed(float s);
	void pause(bool p) { m_pause = p; };
	void vue(dfFileSystem *fs, const std::string& vue, const std::string& component);

	virtual bool update(time_t t);						// update based on timer
	virtual void updateWorldAABB(void);
	virtual void dispatchMessage(gaMessage* message);

	~dfObject3D();
};