#pragma once

#include <glm/glm.hpp>

#include "config.h"
#include "framework/fwApp.h"

class fwCamera;
class fwScene;
class fwSkybox;
class fwLight;

class dfFileSystem;
class dfLevel;

class gaActor;

#ifdef DEBUG
#include "Debugger/Debug.h"
#endif

#include "darkforces/PlayerControl.h"

class myDarkForces : public fwApp
{
	fwLight* m_light = nullptr;
	fwSkybox* m_skybox = nullptr;
	glm::vec4* white = nullptr;
	fwMesh* m_fwCamera = nullptr;

	gaActor* m_player = nullptr;
	DarkForces::PlayerControl* m_playerControl = nullptr;

	dfFileSystem* m_filesystem = nullptr;
	dfLevel* m_level = nullptr;
	bool m_keySpace = false;	// SPACE is pressed
	bool m_headlight = false;
	bool m_f5 = false;

#ifdef DEBUG
	friend Debugger::Debug;
#endif

public:
	myDarkForces(std::string name, int width, int height);
	glTexture* draw(time_t delta, fwRenderer* renderer) override;
	void renderGUI(void) override;

	void resize(int x, int y);
	void keypress(int);
	~myDarkForces();
};