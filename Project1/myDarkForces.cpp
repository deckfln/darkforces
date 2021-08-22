#include "myDarkForces.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>
#include <memory>

#include "config.h"
#include "framework/fwMaterialBasic.h"
#include "framework/fwMesh.h"
#include "framework/geometries/fwBoxGeometry.h"
#include "framework/lights/fwPointLight.h"
#include "framework/controls/fwControlThirdPerson.h"
#include "framework/fwAABBox.h"
#include "framework/fwHUDelement.h"

#include "gaEngine/World.h"
#include "gaEngine/Model.h"
#include "gaEngine/Debug.h"
#include "darkforces/dfLevel.h"
#include "darkforces/dfCollision.h"
#include "darkforces/dfComponent/dfComponentActor.h"
#include "darkforces/dfFileSystem.h"
#include "darkforces/dfActor.h"
#include "darkforces/dfLogicTrigger.h"
#include "darkforces/dfObject/dfSpriteAnimated.h"
#include "darkforces/dfBullet.h"
#include "darkforces/dfObject/dfBulletExplode.h"

const float c_height = 0.70f;
const float c_radius = 0.2f;
const float c_eyes = 0.55f;
const float c_ankle = 0.26f;
const float c_direction = +pi / 1.0f;

myDarkForces::myDarkForces(std::string name, int width, int height) :
	fwApp(name, width, height, "shaders/gamma", "#define GAMMA_CORRECTION 1\n")
{
	int Button = 0;

	m_filesystem = new dfFileSystem(ROOT_FOLDER);

	// camera
	m_camera = new fwCamera(width, height);

	// scene
	m_scene = new fwScene();
	g_gaWorld.scene(m_scene);

#ifdef _DEBUG
	m_debugger = new GameEngine::Debug(this);
#endif

	// player
	//glm::vec3 start = glm::vec3(-21.26f, 0.95f, 29.064f);	// stage
	//glm::vec3 start = glm::vec3(-20.82f, 0.07f, 33.43f);	// westwing
	//glm::vec3 start = glm::vec3(-46, 0.9, 26.8); // super secret
	//glm::vec3 start = glm::vec3(-29.5f, 2.0f, 30.808f);	// gen_d
	//glm::vec3 start = glm::vec3(-20.8f, 0.4f, 29.7f);	// stage
	//glm::vec3 start = glm::vec3(-21.29, -0.3, 16.6);	// post_e
	//glm::vec3 start = glm::vec3(-17.60, -2.0, 27.77);	// projector
	//glm::vec3 start = glm::vec3(-20, 2.0, 34);	// mousebot(40)
	glm::vec3 start = glm::vec3(-28.65f, -2.0, 34.83f);	// spinner
	fwCylinder bounding(glm::vec3(0), c_radius, c_height); // stage


	m_player = new DarkForces::Actor(DF_ENTITY_OBJECT, "player", bounding, start, c_eyes, c_ankle);

	// controls	
	m_playerControl = new DarkForces::PlayerControl(m_camera, m_player, c_direction);
	m_playerControl->setMyDarkForce(this);

	g_gaWorld.addClient(m_player);

	// secret area	m_control = new fwControlThirdPerson(m_camera, glm::vec3(-36.4, 2.3, 37.8), 0.55f, -pi / 2, 0.2f);
	// start m_control = new fwControlThirdPerson(m_camera, glm::vec3(-23.2, 4.3, 29.9), 0.55f, -pi / 2, 0.2f);
	// main room 	m_control = new fwControlThirdPerson(m_camera, glm::vec3(-21.26f, 0.95f, 29.064f), 0.55f, -pi / 4.0f, 0.2f);
	// marr 	m_control = new fwControlThirdPerson(m_camera, glm::vec3(-24, 4.2, 36.3), 0.55f, -pi / 2, 0.2f);
	// super secret		m_control = new fwControlThirdPerson(m_camera, glm::vec3(-46, 0.9, 26.8), 0.55f, -pi / 2, 0.2f);
	// armory	m_control = new fwControlThirdPerson(m_camera, glm::vec3(-37, -2, 36), 0.55f, -pi / 2, 0.2f);

	// lock the view -45° to +45°
	m_playerControl->lockView(M_PI / 4, M_PI / 4 + M_PI / 2);

	bindControl((fwControl*)m_playerControl);

	m_renderer->customLight("/data/shaders/lightning.glsl");

	// pre-load animations
	const std::vector<std::string> preloads = {
		"BULLEXP.WAX",
		"IST-GUNI.FME",
		"IKEYR.FME",
		"IENERGY.FME",
		"IPOWER.FME",
		"DEDMOUSE.FME"
	};
	GameEngine::Model* mod;
	for (const std::string& n: preloads) {
		mod = new GameEngine::Model(n, GameEngine::PRELOAD);
		g_gaWorld.addModel(mod);
	}

	// load first level
	m_level = new dfLevel(m_filesystem, "SECBASE");
	m_player->bind(m_level);

	// hud display
	dfBitmap* bmStatuslt = new dfBitmap(m_filesystem, "STATUSLF.BM", m_level->palette());
	fwTexture* texStatuslt = bmStatuslt->fwtexture();
	fwHUDelement* statuslt = new fwHUDelement("statuslt", fwHUDElementPosition::BOTTOM_LEFT, fwHUDelementSizeLock::UNLOCKED, 0.1f, 0.1f, texStatuslt);

	dfBitmap* bmStatusrt = new dfBitmap(m_filesystem, "STATUSRT.BM", m_level->palette());
	fwTexture* texStatusrt = bmStatusrt->fwtexture();
	fwHUDelement* statusrt = new fwHUDelement("statusrt", fwHUDElementPosition::BOTTOM_RIGHT, fwHUDelementSizeLock::UNLOCKED, 0.1f, 0.1f, texStatusrt);

	dfBitmap* bmRifle = new dfBitmap(m_filesystem, "RIFLE1.BM", m_level->palette());
	fwTexture* texRifle = bmRifle->fwtexture();
	fwHUDelement* rifle = new fwHUDelement("rifle", fwHUDElementPosition::BOTTOM_CENTER, fwHUDelementSizeLock::UNLOCKED, 0.2f, 0.2f, texRifle);

	// init the m_scene
	glm::vec3* yellow = new glm::vec3(255, 255, 0);

	// add the HUD
	m_scene->hud(statuslt);
	m_scene->hud(statusrt);
	m_scene->hud(rifle);

	// mandatory to get all data together
	resizeEvent(width, height);

	// prepare the flight recorder
	g_Blackbox.registerClass("gaEntity", &gaEntity::create);
	g_Blackbox.registerClass("gaActor", &gaActor::create);
	g_Blackbox.registerClass("dfElevator", &dfElevator::create);
	g_Blackbox.registerClass("dfBullet", &dfBullet::create);
	g_Blackbox.registerClass("dfSpriteAnimated", &dfSpriteAnimated::create);
	g_Blackbox.registerClass("dfBulletExplode", &dfBulletExplode::create);

	// prepare the debugger
	static std::map<int32_t, const char*> g_definitions = {
		{DF_MESSAGE_TRIGGER, "DF_TRIGGER"},
		{DF_MESSAGE_GOTO_STOP, "DF_GOTO_STOP"},
		{DF_MESSAGE_DONE, "DF_DONE"},
		{DF_MESSAGE_ADD_SHIELD, "DF_ADD_SHIELD"},
		{DF_MESSAGE_ADD_ENERGY, "DF_ADD_ENERGY"},
		{DF_MESSAGE_HIT_BULLET,	"DF_HIT_BULLET"},
		{DF_MESSAGE_END_LOOP, "DF_END_LOOP"},
		{DF_MESSAGE_DIES, "DF_DIES"},
		{DF_MSG_STATE,	"DF_STATE"},
		{DF_MSG_PICK_RIFLE_AND_BULLETS, "DF_PICK_RIFLE_AND_BULLETS"},
		{DF_MSG_ADD_BATTERY, "DF_ADD_BATTERY"},
		{DF_MSG_EVENT, "DF_EVENT"}
	};

	static std::map<int32_t, const std::map<int32_t, const char*>> g_def = {
		{DF_MSG_EVENT, {
			{DarkForces::CROSSLINE_FRONT, "CROSSLINE_FRONT"},
			{DarkForces::CROSSLINE_BACK, "CROSSLINE_BACK"},
			{DarkForces::ENTER_SECTOR, "ENTER_SECTOR"},
			{DarkForces::LEAVE_SECTOR, "LEAVE_SECTOR"},
			{DarkForces::NUDGE_FRONT_INSIDE, "NUDGE_FRONT_INSIDE"},
			{DarkForces::NUDGE_BACK_OUTSIE, "NUDGE_BACK_OUTSIE"},
			{DarkForces::EXPLOSION, "EXPLOSION"},
			{DarkForces::SHOOT, "SHOOT"},
			{DarkForces::LAND, "LAND"}}
		}
	};

	gaMessage::declareMessages(g_definitions, g_def);
}

void myDarkForces::resize(int width, int height)
{
	m_camera->set_ratio(width, height);
}

static int pFrame = 0;
static int current = 0;

/**
 * Render the world
 */
glTexture* myDarkForces::draw(time_t delta, fwRenderer* renderer)
{
	m_renderer->customDefine("HEADLIGHT", m_headlight);

	m_level->draw(m_camera, m_scene); 	// update visible objects
	g_gaWorld.process(33);

	return m_renderer->draw(m_camera, m_scene);
}

void myDarkForces::keypress(int key)
{
	switch (key) {
	case GLFW_KEY_SPACE: {
		// create a player AA BoundingBox
		glm::vec3 position = m_camera->get_position();
		fwAABBox player(
			position.x - 0.4, position.x + 0.4,
			position.y - 0.3, position.y + 0.3,
			position.z - 0.4, position.z + 0.4
		);

		m_level->testSwitch(player, m_player);
		break;	}
	case GLFW_KEY_F5:
		m_headlight = !m_headlight;
		break;
	}
}

myDarkForces::~myDarkForces()
{
	delete m_camera;
	delete m_light;
	delete m_control;
	delete m_scene;
	delete white;
}