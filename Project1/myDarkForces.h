#pragma once

#include <glm/glm.hpp>

#include "framework/fwApp.h"
#include "framework/fwCamera.h"
#include "framework/lights/fwDirectionLight.h"
#include "framework/lights/fwPointLight.h"
#include "framework/lights/fwSpotLight.h"
#include "framework/fwScene.h"
#include "framework/fwSkybox.h"
#include "framework/controls/fwOrbitControl.h"
#include "framework/controls/fwControlThirdPerson.h"
#include "framework/fwInstancedMesh.h"
#include "framework/fwRenderer.h"
#include "framework/mesh/fwMeshSkinned.h"
#include "framework/fwParticles.h"
#include "darkforces/dfLevel.h"
#include "darkforces/dfFileSystem.h"

class myDarkForces : public fwApp
{
	fwCamera* m_camera = nullptr;
	fwLight* m_light = nullptr;
	fwSkybox* m_skybox = nullptr;
	fwScene* m_scene = nullptr;;
	fwControlThirdPerson* m_control = nullptr;
	glm::vec4* white = nullptr;
	fwMesh* m_fwCamera = nullptr;

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