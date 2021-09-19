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
#include "gaEngine/gaComponent/gaController.h"
#include "gaEngine/gaComponent/gaActiveProbe.h"
#include "gaEngine/gaBoundingBoxes.h"

#include "darkforces/dfConfig.h"
#include "darkforces/dfLevel.h"
#include "darkforces/dfCollision.h"
#include "darkforces/dfComponent/dfComponentActor.h"

#include "darkforces/dfFileLFD.h"
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
const float c_direction = pi/2.0f; // 1.0f;

myDarkForces::myDarkForces(std::string name, int width, int height) :
	GameEngine::App(name, width, height, "shaders/gamma", "#define GAMMA_CORRECTION 1\n")
{
	int Button = 0;
	//DarkForces::FileLFD briefing(ROOT_FOLDER + "/lfd/DFBRIEF.LFD");
	m_filesystem = new dfFileSystem(ROOT_FOLDER);

	// player
	//glm::vec3 start = glm::vec3(-21.26f, 0.95f, 29.064f);	// stage
	//glm::vec3 start = glm::vec3(-20.82f, 0.07f, 33.43f);	// westwing
	//glm::vec3 start = glm::vec3(-46, 0.9, 26.8); // super secret
	//glm::vec3 start = glm::vec3(-29.5f, 2.0f, 30.808f);	// gen_d
	glm::vec3 start = glm::vec3(-22.65f, 2.0f, 22.0);	// gen_d
	//glm::vec3 start = glm::vec3(-20.8f, 0.4f, 29.7f);	// stage
	//glm::vec3 start = glm::vec3(-21.29, -0.3, 16.6);	// post_e
	//glm::vec3 start = glm::vec3(-17.60, -2.0, 27.77);	// projector
	//glm::vec3 start = glm::vec3(-20, 2.0, 34);	// mousebot(40)
	//glm::vec3 start = glm::vec3(-28.65f, -2.0, 34.83f);	// spinner
	//glm::vec3 start = glm::vec3(-28.0287533, -2.0, 27.4463711);	// projector
	//glm::vec3 start = glm::vec3(-56.25, -0.9, 21.65);	// super secret 2
	//glm::vec3 start = glm::vec3(-24.52, 0.07, 31.75);	// enthall

	fwCylinder bounding(glm::vec3(0), c_radius, c_height); // stage

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
	static_cast<dfLevel*>(m_level)->addSkymap(m_scene);

	// hud display
	dfBitmap* bmStatuslt = new dfBitmap(m_filesystem, "STATUSLF.BM", static_cast<dfLevel*>(m_level)->palette());
	fwTexture* texStatuslt = bmStatuslt->fwtexture();
	fwHUDelement* statuslt = new fwHUDelement("statuslt", fwHUDElementPosition::BOTTOM_LEFT, fwHUDelementSizeLock::UNLOCKED, 0.1f, 0.1f, texStatuslt);

	dfBitmap* bmStatusrt = new dfBitmap(m_filesystem, "STATUSRT.BM", static_cast<dfLevel*>(m_level)->palette());
	fwTexture* texStatusrt = bmStatusrt->fwtexture();
	fwHUDelement* statusrt = new fwHUDelement("statusrt", fwHUDElementPosition::BOTTOM_RIGHT, fwHUDelementSizeLock::UNLOCKED, 0.1f, 0.1f, texStatusrt);

	dfBitmap* bmRifle = new dfBitmap(m_filesystem, "RIFLE1.BM", static_cast<dfLevel*>(m_level)->palette());
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

	// and put the player in position
	m_player = new DarkForces::Actor(DarkForces::ClassID::Object, "player", bounding, start, c_eyes, c_ankle);
	const std::vector<uint32_t> keys = {
		GLFW_KEY_X,
		GLFW_KEY_LEFT_CONTROL,
		GLFW_KEY_SPACE,
		GLFW_KEY_S,
		GLFW_KEY_F5
	};
	GameEngine::Component::Controller* controller = new GameEngine::Component::Controller(m_camera, start, c_eyes, c_direction, c_radius, keys);
	bindControl((fwControl*)controller);
	m_player->addComponent(controller, gaEntity::Flag::DONT_DELETE);
	g_gaWorld.addClient(m_player);
	static_cast<DarkForces::Actor*>(m_player)->bind(static_cast<dfLevel*>(m_level));

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

/**
 * Render the world
 */
glTexture* myDarkForces::draw(time_t delta, fwRenderer* renderer)
{
	m_renderer->customDefine("HEADLIGHT", static_cast<DarkForces::Actor*>(m_player)->headlight());

	return GameEngine::App::draw(delta, renderer);
}

myDarkForces::~myDarkForces()
{
	delete m_player;
	delete m_light;
	delete white;
}