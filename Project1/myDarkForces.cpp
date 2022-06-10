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

#include "alEngine/alListener.h"

#include "gaEngine/World.h"
#include "gaEngine/Model.h"
#include "gaEngine/Debug.h"
#include "gaEngine/gaComponent/gaController.h"
#include "gaEngine/gaComponent/gaActiveProbe.h"
#include "gaEngine/gaBoundingBoxes.h"
#include "gaEngine/gaBehavior.h"

#include "darkforces/dfComponent.h"
#include "darkforces/dfConfig.h"
#include "darkforces/dfLevel.h"
#include "darkforces/dfCollision.h"
#include "darkforces/dfComponent/dfCActor.h"
#include "darkforces/dfComponent/dfEnemyAI.h"

#include "darkforces/dfFileLFD.h"
#include "darkforces/dfFileSystem.h"
#include "darkforces/dfPlayer.h"
#include "darkforces/dfLogicTrigger.h"
#include "darkforces/dfBullet.h"
#include "darkforces/dfObject/dfBulletExplode.h"
#include "darkforces/dfPlugin/dfSprites.h"
#include "darkforces/dfVOC.h"
#include "darkforces/dfMsg.h"
#include "darkforces/dfFNT.h"
#include "darkforces/prefab/dfPDA.h"

const float c_height = 0.70f;
const float c_radius = 0.2f;
const float c_eyes = 0.55f;
const float c_ankle = 0.26f;
const float c_direction = pi/2.0f ; // 1.0f;

/**
 * Register al DarkForces behavior nodes on the GameEngine
 */
#include "darkforces/gaBehaviorNode/dfFire2Player.h"
#include "darkforces/gaBehaviorNode/dfWaitDoor.h"
#include "darkforces/gaBehaviorNode/dfWaitIdle.h"
#include "darkforces/gaBehaviorNode/dfBSound.h"
#include "darkforces/gaBehaviorNode/dfBSetVar.h"
#include "darkforces/gaBehaviorNode/dfBActivate.h"
#include "darkforces/gaBehaviorNode/dfBState.h"
 /**
  * register darkforces entities for the flight recorder
  */
void myDarkForces::myDarkForces::registerFRclasses(void)
{
	g_Blackbox.registerClass("dfBullet", &dfBullet::create);
	g_Blackbox.registerClass("dfBulletExplode", &dfBulletExplode::create);
}

/**
 * init the BehaviorTree static plugins
 */
void myDarkForces::myDarkForces::registerBThandlers(void)
{
	GameEngine::Behavior::registerMessage("DarkForces:DYING", DarkForces::Message::DYING);
	GameEngine::Behavior::registerHandler("DarkForces:onDying", reinterpret_cast<GameEngine::Component::BehaviorTree::msgHandler>(&DarkForces::Component::EnemyAI::onDying));
}

/**
 * register darkforces bevavionr nodes for the behavior engine
 */
void myDarkForces::myDarkForces::registerBTNodes(void)
{
	GameEngine::Behavior::registerNode("Fire2Player", DarkForces::Behavior::Fire2Player::create);
	GameEngine::Behavior::registerNode("DarkForces:WaitDoor", DarkForces::Behavior::WaitDoor::create);
	GameEngine::Behavior::registerNode("WaitIdle", DarkForces::Behavior::WaitIdle::create);
	GameEngine::Behavior::registerNode("darkForces:sound", DarkForces::Behavior::Sound::create);
	GameEngine::Behavior::registerNode("DarkForces:SetVar", DarkForces::Behavior::SetVar::create);
	GameEngine::Behavior::registerNode("DarkForces:Activate", DarkForces::Behavior::Activate::create);
	GameEngine::Behavior::registerNode("DarkForces:State", DarkForces::Behavior::State::create);
}

#ifdef _DEBUG
/**
 * register messages & classed for debugger
 */
void myDarkForces::myDarkForces::registerDebugger(void)
{
	static std::map<int32_t, const char*> g_definitions = {
		{DarkForces::Message::TRIGGER, "DF_TRIGGER"},
		{DarkForces::Message::GOTO_STOP, "DF_GOTO_STOP"},
		{DarkForces::Message::DONE, "DF_DONE"},
		{DarkForces::Message::ADD_SHIELD, "DF_ADD_SHIELD"},
		{DarkForces::Message::ADD_ENERGY, "DF_ADD_ENERGY"},
		{DarkForces::Message::DYING, "DF_DYING"},
		{DarkForces::Message::DEAD, "DF_DEAD"},
		{DarkForces::Message::STATE, "dfState"},
		{DarkForces::Message::FORCE_STATE,		"DF_FORCE_STATE"},
		{DarkForces::Message::ADD_BATTERY, "DF_ADD_BATTERY"},
		{DarkForces::Message::EVENT, "DF_EVENT"},
		{DarkForces::Message::CHANGE_WEAPON, "CHANGE_WEAPON"},
		{DarkForces::Message::FIRE, "df_FIRE"},
		{DarkForces::Message::START_FIRE, "df_START_FIRE"},
		{DarkForces::Message::STOP_FIRE, "df_STOP_FIRE"},
		{DarkForces::Message::SatNav_Wait, "dfSatNavWait"},
		{DarkForces::Message::SET_ANIM, "df_setAnim"},
		{DarkForces::Message::ANIM_START, "df_AnimStart"},
		{DarkForces::Message::ANIM_NEXT_FRAME, "df_AnimNextFrame"},
		{DarkForces::Message::ANIM_LASTFRAME, "DF_AnimEndLoop"},
		{DarkForces::Message::ANIM_END, "df_AnimEnd"},
		{DarkForces::Message::ROTATE, "df_ROTATE"},
		{DarkForces::Message::ANIM_PAUSE, "df_AnimPause"},
		{DarkForces::Message::ANIM_VUE, "df_AnimVUE"},
		{DarkForces::Message::MASTER, "df_Master"},
		{DarkForces::Message::COMPLETE, "df_Complete"},
		{DarkForces::Message::WAKEUP, "df_Wakeup"},
		{DarkForces::Message::TEXT,	"dfText"},
		{DarkForces::Message::AMMO,	"dfAmmo"},
		{DarkForces::Message::SHIELD, "dfShield"},
		{DarkForces::Message::LIFE, "dfLife"},
		{DarkForces::Message::PDA,	"dfPda"},
		{DarkForces::Message::PDA_EXIT, "dfPDAExit"},
		{DarkForces::Message::PDA_UP, "dfPDAup"},
		{DarkForces::Message::PDA_DOWN,	"dfPDAdown"},
		{DarkForces::Message::PDA_LEFT, "dfPDAleft}"},
		{DarkForces::Message::PDA_RIGHT, "dfPDAright"},
		{DarkForces::Message::PDA_ZOOM_DOWN,"dfPDAZoomDown"},
		{DarkForces::Message::PDA_ZOOM_UP, "dfPDAZoomUp"},
		{DarkForces::Message::PDA_FLOOR_UP,"dfPDAFloorUp"},
		{DarkForces::Message::PDA_FLOOR_DOWN,"dfPDAFloorDown"},
		{DarkForces::Message::AUTOMAP, "dfAutomap"},
		{DarkForces::Message::AMBIENT, "dfAmbient"},
		{DarkForces::Message::GOGGLES_ON, "dfGogglesOn"},
		{DarkForces::Message::GOGGLES_OFF,"dfGogglesOff"},
		{DarkForces::Message::HEADLIGHT_ON,"dfHeadlightOn"},
		{DarkForces::Message::HEADLIGHT_OFF, "dfHeadlightOff"}
	};

	static std::map<int32_t, const std::map<int32_t, const char*>> g_def = {
		{DarkForces::Message::EVENT, {
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
#endif

static DarkForces::Prefab::PDA *g_pda = nullptr;

/**
 *
 */
myDarkForces::myDarkForces(std::string name, int width, int height) :
	GameEngine::App(name, width, height, "shaders/gamma", "#define GAMMA_CORRECTION 1\n")
{
	int Button = 0;
	m_filesystem = new dfFileSystem(ROOT_FOLDER);

	// preload default LFD files
	DarkForces::FileLFD::load("DFBRIEF");
	DarkForces::FileLFD::load("MENU");

	registerFRclasses();	// register darkforces entities for the flight recorder
	registerBThandlers();	// init the BehaviorTree static plugins
	registerBTNodes();		// register darkforces bevavionr nodes for the behavior engine
#ifdef _DEBUG
	registerDebugger();	// register messages & classed for debugger
#endif

	// player
	//glm::vec3 start = glm::vec3(-21.26f, 0.95f, 29.064f);	// stage
	//glm::vec3 start = glm::vec3(-20.82f, 0.07f, 33.43f);	// westwing
	//glm::vec3 start = glm::vec3(-46, 0.9, 26.8); // super secret
	//glm::vec3 start = glm::vec3(-29.5f, 2.0f, 30.808f);	// gen_d
	glm::vec3 start = glm::vec3(-22.65f, 2.0f, 21.3);	// gen_d
	//glm::vec3 start = glm::vec3(-20.8f, 0.4f, 29.7f);	// stage
	//glm::vec3 start = glm::vec3(-21.29, -0.3, 16.6);	// post_e
	//glm::vec3 start = glm::vec3(-17.60, -2.0, 27.77);	// projector
	//glm::vec3 start = glm::vec3(-20, 2.0, 34);	// mousebot(40)
	//glm::vec3 start = glm::vec3(-28.65f, -2.0, 34.83f);	// spinner
	//glm::vec3 start = glm::vec3(-28.0287533, -2.0, 27.4463711);	// projector
	//glm::vec3 start = glm::vec3(-56.25, -0.9, 23.65);	// super secret 2
	//glm::vec3 start = glm::vec3(-24.52, 0.07, 31.75);	// enthall
	//glm::vec3 start = glm::vec3(-38.80, 2.41, 39.7);	// switch_cover
	//glm::vec3 start = glm::vec3(-29.06, -2.0, 24.75);	// cage
	//glm::vec3 start = glm::vec3(-26.57, 0.0, 29.12);	// elev-3-5
	//glm::vec3 start = glm::vec3(-34, -2.0, 35.654);	// spinner

	fwCylinder bounding(glm::vec3(0), c_radius, c_height); // stage

	// set the sound environement
	g_Listener.maxdistance(20.0f);

	m_renderer->customLight("/darkforces/shaders/lightning.glsl");

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

	// first object of the world is the player
	m_player = new DarkForces::Player(DarkForces::ClassID::_Object, "player", bounding, start, c_eyes, c_ankle);
	const std::vector<GameEngine::Component::Controller::KeyInfo> keys = {
		{GLFW_KEY_X, GameEngine::Component::Controller::KeyInfo::Msg::onPressDown},
		{GLFW_KEY_LEFT_CONTROL, GameEngine::Component::Controller::KeyInfo::Msg::onPress},
		{GLFW_KEY_SPACE, GameEngine::Component::Controller::KeyInfo::Msg::onPressDown},
		{GLFW_KEY_S, GameEngine::Component::Controller::KeyInfo::Msg::onPressDown},
		{GLFW_KEY_F1, GameEngine::Component::Controller::KeyInfo::Msg::onPressDown},	// PDA
		{GLFW_KEY_F3, GameEngine::Component::Controller::KeyInfo::Msg::onPressDown},	// IR goggles
		{GLFW_KEY_F5, GameEngine::Component::Controller::KeyInfo::Msg::onPressDown},	// headlight
		{GLFW_KEY_1, GameEngine::Component::Controller::KeyInfo::Msg::onPressDown},
		{GLFW_KEY_2, GameEngine::Component::Controller::KeyInfo::Msg::onPressDown},
		{GLFW_KEY_TAB, GameEngine::Component::Controller::KeyInfo::Msg::onPressDown}	// automap
	};

	// add a controller to the player
	GameEngine::Component::Controller* controller = new GameEngine::Component::Controller(m_camera, start, c_eyes, c_direction, c_radius, keys);
	bindControl((fwControl*)controller);
	m_player->addComponent(controller, gaEntity::Flag::DONT_DELETE);

	GameEngine::World::add(m_player);

	// PDA
	g_pda = new DarkForces::Prefab::PDA();
	GameEngine::World::add(g_pda);

	// load the text file
	g_dfMsg.Parse("TEXT.MSG");

	// load first level
	m_level = new dfLevel(g_dfFiles, "SECBASE");
	static_cast<dfLevel*>(m_level)->addSkymap(m_scene);

	// extract the automap from the player, to be added on the hud
	DarkForces::Component::AutoMap* automap = static_cast<DarkForces::Player*>(m_player)->automap();
	automap->set(static_cast<dfLevel*>(m_level));

	// mandatory to get all data together
	resizeEvent(width, height);

	m_scene->addMesh2D(automap);							// add the automap on the HUD

	// PDA GUI
	m_scene->addMesh2D(g_pda->ui());

	// load needed data on screen
	static_cast<DarkForces::Player*>(m_player)->setScene(m_scene);

	// add the sprites manager to the world
	g_gaWorld.registerPlugin(&g_dfSpritesEngine);
	g_dfSpritesEngine.OnWorldInsert();

	// and put the player in position
	static_cast<DarkForces::Player*>(m_player)->bind(static_cast<dfLevel*>(m_level));

	// init the m_scene
	glm::vec3* yellow = new glm::vec3(255, 255, 0);

}

/**
 * Render the world
 */
glTexture* myDarkForces::draw(time_t delta, fwRenderer* renderer)
{
	DarkForces::Player* p = static_cast<DarkForces::Player*>(m_player);

	m_renderer->customDefine("HEADLIGHT", p->isOn("headlight"));
	m_renderer->customDefine("GREEN", p->isOn("goggles"));

	return GameEngine::App::draw(delta, renderer);
}

myDarkForces::~myDarkForces()
{
	delete m_player;
	delete m_light;
	delete white;

	DarkForces::deleteVOCcache();
}