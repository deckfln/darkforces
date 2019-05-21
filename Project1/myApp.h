#pragma once

#include <glm/glm.hpp>

#include "framework/fwApp.h"
#include "framework/Camera.h"
#include "framework/DirectionLight.h"
#include "framework/PointLight.h"
#include "framework/SpotLight.h"
#include "framework/fwScene.h"
#include "framework/fwSkybox.h"
#include "framework/controls/fwOrbitControl.h"
#include "framework/fwInstancedMesh.h"

class myApp : public fwApp
{
	Camera *camera;
	PointLight *light;
	SpotLight *light2;
	fwSkybox *skybox;
	fwScene *scene;
	fwOrbitControl *control;
	glm::vec4 *yellow;
	glm::vec4 *white;
	glm::mat4 positions[2];
	fwInstancedMesh *instancedMesh;

public:
	myApp(std::string name,  int width, int height);
	void draw(void);
	void resize(int x, int y);
	~myApp();
};