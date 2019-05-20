#include "myApp.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>

#include "include/stb_image.h"
#include "framework/fwDiffuseMaterial.h"
#include "framework/fwMaterialBasic.h"
#include "framework/fwMesh.h"
#include "framework/fwInstancedMesh.h"
#include "framework/BoxGeometry.h"
#include "framework/geometries/fwPlaneGeometry.h"

#include "framework/Loader.h"

myApp::myApp(std::string name, int width, int height) :
	fwApp(name, width, height)
{
	camera = new Camera(width, height);
	int Button = 0;

	control = new fwOrbitControl(camera);
	bindControl(control);

	/*
	 * Camera
	 */
	camera->lookAt(0, 2, 0);
	camera->translate(0, 3.5, 3.5);

	/*
	 * Lights
	 */
	 /*
	 DirectionLight light(
		 glm::vec3(0.0),
		 glm::vec3(0.2f, 0.2f, 0.2f),
		 glm::vec3(0.5f, 0.5f, 0.5f),
		 glm::vec3(1.0f, 1.0f, 1.0f)
	 );
	 */

	light = new PointLight(
		glm::vec3(),
		glm::vec3(0, 0.2f, 0),
		glm::vec3(0, 0.5f, 0),
		glm::vec3(0, 1.0f, 0),
		1.0,
		0.09,
		0.032
	);

	light2 = new SpotLight(
		glm::vec3(0),
		glm::vec3(0, 0, -1),

		glm::vec3(0.2f, 0, 0),
		glm::vec3(0.5f, 0, 0),
		glm::vec3(1.0f, 1, 1),
		1.0,
		0.09,
		0.032,
		glm::cos(glm::radians(12.5f)),
		glm::cos(glm::radians(17.5f))
	);

	Loader loader("models/stormtrooper/stormtrooper.dae");
	std::vector<fwMesh *>meshes = loader.get_meshes();

	// lights
	BoxGeometry *geometry = new BoxGeometry();
	glm::vec4 *white = new glm::vec4(1.0);

	fwMaterialBasic *basic = new fwMaterialBasic(white);
	fwMesh *fLight = new fwMesh(geometry, basic);
	fwMesh *fLight2 = new fwMesh(geometry, basic);

	glm::vec3 half(0.1);
	fLight->set_scale(half).set_name("light");
	fLight2->set_scale(half).set_name("light2");
	fLight->draw_wireframe(true);

	light->addChild(fLight);
	light2->addChild(fLight2);

	// floor
	Texture *t1 = new Texture("images/container2.png");
	Texture *t2 = new Texture("images/container2_specular.png");
	fwDiffuseMaterial *material = new fwDiffuseMaterial(t1, nullptr, 64);

	fwMesh *plane = new fwMesh(new fwPlaneGeometry(8, 8), material);

	glm::vec3 deg(3.1415 / 2, 0, 0);
	plane->rotate(deg);

	// init the scene
	yellow = new glm::vec4(255, 255, 0, 255);
	scene = new fwScene();
	scene->addLight(light).
		addLight(light2).
		setOutline(yellow).
		addChild(plane);

	glm::mat4 positions[2];
	positions[0] = glm::translate(glm::vec3(-1,0, 0));
	positions[1] = glm::translate(glm::vec3(1, 0, 0));

	for (auto mesh : meshes) {
		fwInstancedMesh *instancedMesh = new fwInstancedMesh(mesh->get_geometry(), mesh->get_material(), 2, positions);
		//mesh->show_normalHelper(true);
		instancedMesh->outline(true);
		scene->addChild(instancedMesh);
	}

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
	glm::vec3 lightPos;

	float radius = 2;
	lightPos.x = sin(glfwGetTime() / 2) * radius;
	lightPos.y = 1;
	lightPos.z = cos(glfwGetTime() / 2) * radius;
	light->translate(lightPos);

	lightPos.x = sin(glfwGetTime() / 4) * radius;
	lightPos.y = 3;
	lightPos.z = cos(glfwGetTime() / 4) * radius;
	light2->translate(lightPos);

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
}