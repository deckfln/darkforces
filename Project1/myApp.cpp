#include "myApp.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>

#include "include/stb_image.h"
#include "framework/lights/fwDirectionLight.h"
#include "framework/fwDiffuseMaterial.h"
#include "framework/fwMaterialBasic.h"
#include "framework/fwMesh.h"
#include "framework/fwInstancedMesh.h"
#include "framework/geometries/fwBoxGeometry.h"
#include "framework/geometries/fwPlaneGeometry.h"

#include "framework/Loader.h"

myApp::myApp(std::string name, int width, int height) :
	fwApp(name, width, height)
{
	camera = new fwCamera(width, height);
	int Button = 0;

	control = new fwOrbitControl(camera);
	bindControl(control);

	/*
	 * fwCamera
	 */
	camera->lookAt(0, 0, 0);
	camera->translate(0, 0, 3);

	/*
	 * Lights
	 */
	 fwDirectionLight *light = new fwDirectionLight(
		 glm::vec3(-2.0f, 4.0f, -1.0f),
		 glm::vec3(0.1, 0.1, 0.1),
		 glm::vec3(0.8f, 0.8f, 0.8f),
		 glm::vec3(0.7f, 0.7f, 0.7f)
	 );
	 light->castShadow(true);

	// lights
	fwBoxGeometry *geometry = new fwBoxGeometry();
	glm::vec4 *white = new glm::vec4(1.0);

	fwMaterialBasic *basic = new fwMaterialBasic(white);
	fwMesh *fLight = new fwMesh(geometry, basic);

	glm::vec3 half(0.1);
	fLight->set_scale(half).set_name("light");

	light->addChild(fLight);

	// floor
	Texture *t1 = new Texture("images/wood.png");
	fwDiffuseMaterial *material = new fwDiffuseMaterial(t1, nullptr, 32);

	fwMesh *plane = new fwMesh(new fwPlaneGeometry(10, 10), material);
	plane->set_name("floor");
	plane->receiveShadow(true);

	// box1
	t1 = new Texture("images/container2.png");
	Texture *t2 = new Texture("images/container2_specular.png");
	material = new fwDiffuseMaterial(t1, nullptr, 32);

	fwMesh *box1 = new fwMesh(geometry, material);
	box1->translate(0.5, 0.5, 0);
	box1->castShadow(true);
	box1->set_name("box1");
	box1->receiveShadow(true);

	// box2
	fwMesh *box2 = new fwMesh(geometry, material);
	box2->translate(2, 0, 1);
	box2->castShadow(true);
	box2->set_name("box2");
	box2->receiveShadow(true);

	// box3
	fwMesh *box3 = new fwMesh(geometry, material);
	glm::vec3 r(glm::radians(60.0f), 0, glm::radians(60.0f));
	box3->set_scale(0.25).rotate(r).translate(-1, 0, 2);
	box3->castShadow(true);
	box3->set_name("box3");
	box3->receiveShadow(true);

	glm::vec3 deg(-3.1415 / 2, 0, 0);
	plane->rotate(deg);
	plane->show_normalHelper(true);
	glm::vec3 tr(0, -0.5, 0);
	plane->translate(tr);

	// init the scene
	yellow = new glm::vec4(255, 255, 0, 255);
	scene = new fwScene();
	scene->addLight(light).
		setOutline(yellow).
		addChild(plane).
		addChild(box1).
		addChild(box2).
		addChild(box3);

	positions[0] = glm::translate(glm::vec3(-1,0, 0));
	positions[1] = glm::translate(glm::vec3(1, 0, 0));

	// Skybox
	std::string skyboxes[] = {
		"images/skybox/right.jpg",
		"images/skybox/left.jpg",
		"images/skybox/top.jpg",
		"images/skybox/bottom.jpg",
		"images/skybox/front.jpg",
		"images/skybox/back.jpg" };

	skybox = new fwSkybox(skyboxes);
}

void myApp::resize(int width, int height)
{
	camera->set_ratio(width, height);
}

void myApp::draw(void)
{
	scene->draw(camera);

	skybox->draw(camera);
}

myApp::~myApp()
{
	delete camera;
	delete light;
	delete light2;
	delete skybox;
	delete control;
	delete scene;
	delete yellow;
	delete white;
	delete instancedMesh;
}