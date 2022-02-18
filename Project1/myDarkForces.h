#pragma once

#include <glm/glm.hpp>

#include "config.h"
#include "gaEngine/gaApp.h"

class fwCamera;
class fwScene;
class fwSkybox;
class fwLight;

class dfFileSystem;
class dfLevel;
class gaEntity;

#include "darkforces/dfPlayer.h"

namespace DarkForces {
	class HUD;
}

class myDarkForces : public GameEngine::App
{
	fwLight* m_light = nullptr;
	fwSkybox* m_skybox = nullptr;
	glm::vec4* white = nullptr;

	dfFileSystem* m_filesystem = nullptr;

	bool m_keySpace = false;	// SPACE is pressed
	bool m_headlight = false;
	bool m_f5 = false;

	void registerFRclasses(void);	// register darkforces entities for the flight recorder
	void registerBThandlers(void);	// init the BehaviorTree static plugins
	void registerBTNodes(void);	// register darkforces bevavionr nodes for the behavior engine

#ifdef _DEBUG
	void registerDebugger(void) override;	// register messages & classed for debugger
#endif

public:
	myDarkForces(std::string name, int width, int height);
	glTexture* draw(time_t delta, fwRenderer* renderer) override;

	~myDarkForces();
};