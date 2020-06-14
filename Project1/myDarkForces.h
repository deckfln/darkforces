#pragma once

#include <glm/glm.hpp>

#include "framework/fwApp.h"

class fwCamera;
class fwScene;
class fwSkybox;
class fwLight;

class dfFileSystem;
class dfLevel;

class gaActor;
class gaPlayer;

class myDarkForces : public fwApp
{
	fwCamera* m_camera = nullptr;
	fwLight* m_light = nullptr;
	fwSkybox* m_skybox = nullptr;
	fwScene* m_scene = nullptr;;
	glm::vec4* white = nullptr;
	fwMesh* m_fwCamera = nullptr;

	gaActor* m_player = nullptr;
	gaPlayer* m_control = nullptr;

	dfFileSystem* m_filesystem = nullptr;
	dfLevel* m_level = nullptr;
	bool m_keySpace = false;	// SPACE is pressed
	bool m_headlight = false;
	bool m_f5 = false;

public:
	myDarkForces(std::string name, int width, int height);
	glTexture* draw(time_t delta, fwRenderer* renderer);
	void resize(int x, int y);
	void keypress(void);
	~myDarkForces();
};