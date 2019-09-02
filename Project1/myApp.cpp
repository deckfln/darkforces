#include "myApp.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>

#include "include/stb_image.h"
#include "framework/lights/fwDirectionLight.h"
#include "framework/fwMaterialDiffuse.h"
#include "framework/fwMaterialBasic.h"
#include "framework/fwMesh.h"
#include "framework/fwInstancedMesh.h"
#include "framework/geometries/fwBoxGeometry.h"
#include "framework/geometries/fwPlaneGeometry.h"
#include "framework/fwSprite.h"

#include "framework/Loader.h"

const std::string root = "D:/dev/project1/project1/";

myApp::myApp(std::string name, int width, int height) :
	fwApp(name, width, height, "shaders/gamma", "#define GAMMA_CORRECTION 1\n")
{
	m_camera = new fwCamera(width, height);
	int Button = 0;

	m_control = new fwOrbitControl(m_camera);
	bindControl(m_control);

	/*
	 * fwCamera
	 */
	m_camera->translate(-6.859210, 20.333462, 22.371893);

	/*
	 * Lights
	 */
	m_light = new fwDirectionLight(
		glm::vec3(),
		glm::vec3(0.3, 0.3, 0.3),
		glm::vec3(0.8, 0.8, 0.8),
		glm::vec3(1.0, 1.0, 1.0)
	);
	m_light->set_name("light");
	m_light->castShadow(true);

	// lights
	fwBoxGeometry *geometry = new fwBoxGeometry();
	glm::vec4 *white = new glm::vec4(0.0, 0.0, 1.0, 1.0);

	fwMaterialBasic *basic = new fwMaterialBasic(white);
	fwMesh *fLight = new fwMesh(geometry, basic);

	glm::vec3 half(0.5);
	fLight->set_scale(half).set_name("light_impersonator");

	m_light->addChild(fLight);

	// floor
	fwTexture *t1 = new fwTexture(root+"images/brickwall.jpg");
	fwTexture *t2 = new fwTexture(root+"images/brickwall_normal.jpg");
	fwMaterialDiffuse *material = new fwMaterialDiffuse(t1, 64);
	material->normalMap(t2);

	fwMesh *plane = new fwMesh(new fwPlaneGeometry(10, 10), material);
	plane->set_name("floor");
	plane->receiveShadow(true);

	glm::vec3 deg(-3.1415 / 2, 0, 0);
	plane->rotate(deg);
	glm::vec3 tr1(0, -0.5, 0);
	plane->translate(tr1);

	// window
	t1 = new fwTexture(root+"images/blending_transparent_window.png");
	material = new fwMaterialDiffuse(t1, 32);

	fwMesh *window = new fwMesh(new fwPlaneGeometry(5, 5), material);
	window->set_name("window");
	glm::vec3 tr(3, 3, 3);
	window->translate(tr);
	window->transparent(true);

	// box
	t1 = new fwTexture(root+"images/container2.png");
	t2 = new fwTexture(root+"images/container2_specular.png", 1);	// specular maps only need 1 channel

	material = new fwMaterialDiffuse(t1, 32);
	material->specularMap(t2);

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

	m_positions[0] = glm::translate(glm::vec3(-1,0, 0));
	m_positions[1] = glm::translate(glm::vec3(1, 0, 0));

	// model
	Loader* loader = new Loader(root+"models/stormtrooper/stormtrooper.dae");
	m_stormtrooper = (fwMeshSkinned *)loader->get_meshes()[0];
	glm::vec3 rot(-pi / 2, 0, 0);
	m_stormtrooper->rotate(rot);
	glm::vec3 v(-2, 0, 0);
	m_stormtrooper->translate(v);
	m_stormtrooper->run("");

	// sprite
	glm::vec3 *sprites = new glm::vec3 [4];
	sprites[0].z = -4;
	sprites[1].z = -2;
	sprites[2].z = 0;
	sprites[3].z = 2;
	t1 = new fwTexture(root+"images/fireworks_red.jpg");

	fwSprite *sprite = new fwSprite(sprites, 4, t1);

	sprite->set_name("sprite");

	// Skybox
	std::string skyboxes[] = {
		"images/skybox/right.jpg",
		"images/skybox/left.jpg",
		"images/skybox/top.jpg",
		"images/skybox/bottom.jpg",
		"images/skybox/front.jpg",
		"images/skybox/back.jpg" };

	m_skybox = new fwSkybox(skyboxes);

	// init the m_scene
	glm::vec3 *yellow = new glm::vec3(255, 255, 0);
	m_scene = new fwScene();
	m_scene->addLight(m_light).
		setOutline(yellow).
		addChild(plane).
		addChild(m_instancedMesh).
		addChild(window).
		addChild(sprite).
		addChild(m_stormtrooper);
	m_scene->background(m_skybox);
}

void myApp::resize(int width, int height)
{
	m_camera->set_ratio(width, height);
}

glTexture *myApp::draw(time_t delta, fwRenderer *renderer)
{
	m_positions[0] = glm::translate(glm::vec3(1, sin(glfwGetTime() / 2) * 2, 0));
	m_instancedMesh->update_position(0, 1);

	glm::vec3 lightPos;

	float radius = 2;
	lightPos.x = sin(glfwGetTime() / 2) * radius;
	lightPos.y = 4;
	lightPos.z = cos(glfwGetTime() / 2) * radius;
	m_light->translate(lightPos);
	m_stormtrooper->update(delta);

	return renderer->draw (m_camera, m_scene);
}

void myApp::keypress()
{
}

myApp::~myApp()
{
	delete m_camera;
	delete m_light;
	delete m_skybox;
	delete m_control;
	delete m_scene;
	delete white;
	delete m_instancedMesh;
	delete m_stormtrooper;
}