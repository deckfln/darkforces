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
#include "framework/fwInstancedMesh.h"
#include "framework/fwRenderer.h"
#include "framework/mesh/fwMeshSkinned.h"
#include "framework/fwParticles.h"

class myApp : public fwApp
{
	fwCamera *m_camera = nullptr;
	fwDirectionLight *m_light = nullptr;
	fwSkybox *m_skybox = nullptr;
	fwScene* m_scene = nullptr;;
	fwOrbitControl *m_control = nullptr;
	glm::vec4 *white = nullptr;
	glm::mat4 m_positions[3];
	fwInstancedMesh *m_instancedMesh = nullptr;
	fwMeshSkinned* m_stormtrooper = nullptr;
	fwMesh* m_fwCamera = nullptr;
	fwParticles* m_particles = nullptr;

public:
	myApp(std::string name,  int width, int height);
	glTexture *draw(time_t delta, fwRenderer *renderer);
	void resize(int x, int y);
	void keypress(void);
	~myApp();
};