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
	 /*
	 fwDirectionLight light(
		 glm::vec3(0.0),
		 glm::vec3(0.2f, 0.2f, 0.2f),
		 glm::vec3(0.5f, 0.5f, 0.5f),
		 glm::vec3(1.0f, 1.0f, 1.0f)
	 );
	 */

	light = new fwPointLight(
		glm::vec3(),
		glm::vec3(0, 0.2f, 0),
		glm::vec3(0, 0.5f, 0),
		glm::vec3(0, 1.0f, 0),
		1.0,
		0.09,
		0.032
	);

	fwPointLight *light3 = new fwPointLight(
		glm::vec3(0, 0, 0),
		glm::vec3(0.01, 0.01, 0.01),	// ambient
		glm::vec3(1, 1, 1),				// diffuse
		glm::vec3(0.3, 0.3f, 0.3),		// specular
		1.0,							// attenuation constant
		0.00,							// attenuation linear
		0.0								// attenuation quadatic
	);

	light2 = new fwSpotLight(
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
	fwBoxGeometry *geometry = new fwBoxGeometry();
	glm::vec4 *white = new glm::vec4(1.0);

	fwMaterialBasic *basic = new fwMaterialBasic(white);
	fwMesh *fLight = new fwMesh(geometry, basic);
	fwMesh *fLight2 = new fwMesh(geometry, basic);

	glm::vec3 half(0.1);
	fLight->set_scale(half).set_name("light");
	fLight2->set_scale(half).set_name("light2");
	fLight->draw_wireframe(true);

	light->addChild(fLight);
	light3->addChild(fLight2);

	// floor
	Texture *t1 = new Texture("images/wood.png");
	Texture *t2 = new Texture("images/container2_specular.png");
	fwDiffuseMaterial *material = new fwDiffuseMaterial(t1, nullptr, 32);

	fwMesh *plane = new fwMesh(new fwPlaneGeometry(10, 10), material);

	glm::vec3 deg(-3.1415 / 2, 0, 0);
	plane->rotate(deg);
	plane->show_normalHelper(true);
	glm::vec3 tr(0, -0.5, 0);
	plane->translate(tr);

	// init the scene
	yellow = new glm::vec4(255, 255, 0, 255);
	scene = new fwScene();
	scene->addLight(light3).
		setOutline(yellow).
		addChild(plane);

	positions[0] = glm::translate(glm::vec3(-1,0, 0));
	positions[1] = glm::translate(glm::vec3(1, 0, 0));

	fwMesh *mesh = meshes[0];
	instancedMesh = new fwInstancedMesh(mesh->get_geometry(), mesh->get_material(), 2, positions);
	
	//mesh->show_normalHelper(true);
	instancedMesh->outline(true);
	//scene->addChild(instancedMesh);

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
	// move instanced model 2
	positions[1] = glm::translate(glm::vec3(1, sin(glfwGetTime() / 2) * 2, 0));
	instancedMesh->update_position(1, 1);

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
	delete instancedMesh;
}