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

class myApp : public fwApp
{
	fwCamera *m_camera;
	fwPointLight *m_light;
	fwSkybox *m_skybox;
	fwScene *m_scene;
	fwOrbitControl *m_control;
	glm::vec4 *white;
	glm::mat4 m_positions[3];
	fwInstancedMesh *m_instancedMesh;

public:
	myApp(std::string name,  int width, int height);
	void draw(fwForwardRenderer *renderer);
	void resize(int x, int y);
	~myApp();
};