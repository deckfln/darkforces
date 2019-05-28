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
	fwApp(name, width, height, "shaders/gamma", "#define GAMMA_CORRECTION 2.2\n")
{
	m_camera = new fwCamera(width, height);
	int Button = 0;

	m_control = new fwOrbitControl(m_camera);
	bindControl(m_control);

	/*
	 * fwCamera
	 */
	m_camera->lookAt(0, 0, 0);
	m_camera->translate(0, 0, 3);

	/*
	 * Lights
	 */
	 m_light = new fwDirectionLight(
		 glm::vec3(-2.0f, 4.0f, -1.0f),
		 glm::vec3(0.2, 0.2, 0.2),
		 glm::vec3(0.8f, 0.8f, 0.8f),
		 glm::vec3(0.7f, 0.7f, 0.7f)
	 );
	 m_light->castShadow(true);

	// lights
	fwBoxGeometry *geometry = new fwBoxGeometry();
	glm::vec4 *white = new glm::vec4(1.0);

	fwMaterialBasic *basic = new fwMaterialBasic(white);
	fwMesh *fLight = new fwMesh(geometry, basic);

	glm::vec3 half(0.1);
	fLight->set_scale(half).set_name("light");

	m_light->addChild(fLight);

	// floor
	fwTexture *t1 = new fwTexture("images/wood.png");
	fwDiffuseMaterial *material = new fwDiffuseMaterial(t1, nullptr, 32);

	fwMesh *plane = new fwMesh(new fwPlaneGeometry(10, 10), material);
	plane->set_name("floor");
	plane->receiveShadow(true);

	glm::vec3 deg(-3.1415 / 2, 0, 0);
	plane->rotate(deg);
	glm::vec3 tr(0, -0.5, 0);
	plane->translate(tr);

	// box
	t1 = new fwTexture("images/container2.png");
	fwTexture *t2 = new fwTexture("images/container2_specular.png");

	material = new fwDiffuseMaterial(t1, nullptr, 32);

	m_positions[0] = glm::translate(glm::vec3(0.5, 0.5, 0.5));
	m_positions[1] = glm::translate(glm::vec3(2, 0, 1));

	glm::mat4 rotationMatrix = glm::rotate(glm::radians(60.0f), glm::vec3(1, 0, 0));
	glm::mat4 scaleMatrix = glm::scale(glm::vec3(0.25, 0.25, 0.25));
	glm::mat4 translateMatrix = glm::translate(glm::vec3(-1, 0, 2));

	m_positions[2] = translateMatrix * scaleMatrix * rotationMatrix;

	m_instancedMesh = new fwInstancedMesh(geometry, material, 3, m_positions);
	m_instancedMesh->translate(0.5, 0.5, 0);
	m_instancedMesh->castShadow(true);
	m_instancedMesh->set_name("box1");
	m_instancedMesh->receiveShadow(true);

	// init the m_scene
	yellow = new glm::vec4(255, 255, 0, 255);
	m_scene = new fwScene();
	m_scene->addLight(m_light).
		setOutline(yellow).
		addChild(plane).
		addChild(m_instancedMesh);

	m_positions[0] = glm::translate(glm::vec3(-1,0, 0));
	m_positions[1] = glm::translate(glm::vec3(1, 0, 0));

	// Skybox
	std::string skyboxes[] = {
		"images/skybox/right.jpg",
		"images/skybox/left.jpg",
		"images/skybox/top.jpg",
		"images/skybox/bottom.jpg",
		"images/skybox/front.jpg",
		"images/skybox/back.jpg" };

	m_skybox = new fwSkybox(skyboxes);
}

void myApp::resize(int width, int height)
{
	m_camera->set_ratio(width, height);
}

void myApp::draw(void)
{
	m_positions[0] = glm::translate(glm::vec3(1, sin(glfwGetTime() / 2) * 2, 0));
	m_instancedMesh->update_position(0, 1);

	glm::vec3 lightPos;

	float radius = 2;
	lightPos.x = sin(glfwGetTime() / 2) * radius;
	lightPos.y = 4;
	lightPos.z = cos(glfwGetTime() / 2) * radius;
	m_light->translate(lightPos);

	m_scene->draw(m_camera);

	m_skybox->draw(m_camera);
}

myApp::~myApp()
{
	delete m_camera;
	delete m_light;
	delete m_skybox;
	delete m_control;
	delete m_scene;
	delete yellow;
	delete white;
	delete m_instancedMesh;
}