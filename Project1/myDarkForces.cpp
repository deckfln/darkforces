#include "myDarkForces.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>

#include "config.h"
#include "include/stb_image.h"
#include "framework/lights/fwDirectionLight.h"
#include "framework/fwMaterialDiffuse.h"
#include "framework/fwMaterialBasic.h"
#include "framework/fwMesh.h"
#include "framework/fwInstancedMesh.h"
#include "framework/geometries/fwBoxGeometry.h"
#include "framework/geometries/fwPlaneGeometry.h"
#include "framework/fwParticles.h"

#include "framework/Loader.h"

#include "darkforces/dfLevel.h"
#include "darkforces/dfCollision.h"

myDarkForces::myDarkForces(std::string name, int width, int height) :
	fwApp(name, width, height, "shaders/gamma", "#define GAMMA_CORRECTION 1\n")
{
	int Button = 0;

	// camera
	m_camera = new fwCamera(width, height);

	// controls
	m_control = new fwControlThirdPerson(m_camera, glm::vec3(-24, 0.5, 28), glm::vec3(1, 0, 0));
	bindControl((fwControl*)m_control);

	// shared geometry
	fwBoxGeometry* geometry = new fwBoxGeometry();

	/*
	 * Lights
	 */
	 /*
	 m_light = new fwDirectionLight(
		 glm::vec3(),
		 glm::vec3(0.3, 0.3, 0.3),
		 glm::vec3(0.8, 0.8, 0.8),
		 glm::vec3(1.0, 1.0, 1.0)
	 );
	 */
	m_light = new fwPointLight(
		glm::vec3(),				// position
		glm::vec3(0.8, 0.8, 0.8),	// Color
		glm::vec3(0.9, 0.9, 0.9),	// Diffuse
		glm::vec3(1.0, 1.0, 1.0),	// Specular
		1.0,						// constant
		0.09,						// linear
		0.032						// quadratic
	);
	m_light->set_name("light");
	m_light->castShadow(true);

	glm::vec4* white = new glm::vec4(0.0, 0.0, 1.0, 1.0);

	fwMaterialBasic* basic = new fwMaterialBasic(white);
	fwMesh* fLight = new fwMesh(geometry, basic);

	m_level = new dfLevel("data/secbase.lev");
	dfCollision* m_collision = new dfCollision();
	m_collision->bind(m_level);
	m_control->bind(m_collision);

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
	glm::vec3* yellow = new glm::vec3(255, 255, 0);
	m_scene = new fwScene();
	m_scene->addLight(m_light);

	m_scene->background(m_skybox);

	// mandatory to get all data together
	resizeEvent(width, height);
}

void myDarkForces::resize(int width, int height)
{
	m_camera->set_ratio(width, height);
}

static time_t progress[] = {
33,
		62,
		4,
		33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33,
	33
};
static int pFrame = 0;
static int current = 0;

glTexture* myDarkForces::draw(time_t delta, fwRenderer* renderer)
{
	glm::vec3 lightPos;

	float radius = 4;
	lightPos.x = sin(glfwGetTime() / 2) * radius;
	lightPos.y = 2;
	lightPos.z = cos(glfwGetTime() / 2) * radius;
	m_light->translate(lightPos);

	// update visible level sectors
	m_level->draw(m_camera, m_scene);

	return renderer->draw(m_camera, m_scene);
}

void myDarkForces::keypress()
{
}

myDarkForces::~myDarkForces()
{
	delete m_camera;
	delete m_light;
	delete m_skybox;
	delete m_control;
	delete m_scene;
	delete white;
}