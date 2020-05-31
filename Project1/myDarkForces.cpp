#include "myDarkForces.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>

#include "config.h"
#include "framework/fwMaterialBasic.h"
#include "framework/fwMesh.h"
#include "framework/geometries/fwBoxGeometry.h"
#include "framework/lights/fwPointLight.h"
#include "framework/controls/fwControlThirdPerson.h"
#include "framework/fwSkybox.h"
#include "framework/fwAABBox.h"

#include "darkforces/dfLevel.h"
#include "darkforces/dfCollision.h"
#include "darkforces/dfMessageBus.h"

#include "darkforces/dfFileSystem.h"

myDarkForces::myDarkForces(std::string name, int width, int height) :
	fwApp(name, width, height, "shaders/gamma", "#define GAMMA_CORRECTION 1\n")
{
	int Button = 0;

	m_filesystem = new dfFileSystem(ROOT_FOLDER);

	// camera
	m_camera = new fwCamera(width, height);

	// controls
	// secret area 	m_control = new fwControlThirdPerson(m_camera, glm::vec3(-36.4, 2.3, 37.8), 0.55f, -pi / 2, 0.2f);
	// start m_control = new fwControlThirdPerson(m_camera, glm::vec3(-23.2, 4.3, 29.9), 0.55f, -pi / 2, 0.2f);
	// main room 	
	m_control = new fwControlThirdPerson(m_camera, glm::vec3(-21.26f, 0.95f, 29.064f), 0.55f, -pi / 4.0f, 0.2f);
	// marr 	m_control = new fwControlThirdPerson(m_camera, glm::vec3(-24, 4.2, 36.3), 0.55f, -pi / 2, 0.2f);
	// super secret	m_control = new fwControlThirdPerson(m_camera, glm::vec3(-46, 0.9, 26.8), 0.55f, -pi / 2, 0.2f);

	bindControl((fwControl*)m_control);

	m_renderer->customLight("/data/shaders/lightning.glsl");

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
	

	m_light = new fwPointLight(
		glm::vec3(),				// position
		glm::vec3(0.8, 0.8, 0.8),	// Color
		glm::vec3(0.9, 0.9, 0.9),	// Diffuse
		glm::vec3(1.0, 1.0, 1.0),	// Specular
		1.0f,						// constant
		0.09f,						// linear
		0.032f						// quadratic
	);
	m_light->set_name("light");
	m_light->castShadow(true);
	*/

	glm::vec4* white = new glm::vec4(0.0, 0.0, 1.0, 1.0);

	fwMaterialBasic* basic = new fwMaterialBasic(white);
	fwMesh* fLight = new fwMesh(geometry, basic);

	m_level = new dfLevel(m_filesystem, "SECBASE");
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

	// create a player AA BoundingBox
	glm::vec3 position = m_camera->get_position();
	fwAABBox player(
		position.x - 0.4, position.x + 0.4,
		position.y - 0.3, position.y + 0.3,
		position.z - 0.4, position.z + 0.4
		);

	// Space key can only be sent ONCE
	if (m_control->isKeyPressed(GLFW_KEY_SPACE)) {
		if (!m_keySpace) {
			m_level->testSwitch(player);
			m_keySpace = true;
		}
	}
	else {
		m_keySpace = false;
	}

	// DEBUG : suspend the timer
	if (m_control->isKeyPressed(GLFW_KEY_S)) {
		g_MessageBus.suspendTimer();
	}

	// get the status of the headlight
	if (m_control->isKeyPressed(GLFW_KEY_F5)) {
		if (!m_f5) {
			m_headlight = !m_headlight;
			m_f5 = true;
		}
	}
	else {
		m_f5 = false;
	}

	m_renderer->customDefine("HEADLIGHT", m_headlight);

	m_level->draw(m_camera, m_scene); 	// update visible objects
	g_MessageBus.process(33);

	m_control->updateCamera(33);			// and move the player if the level changed

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