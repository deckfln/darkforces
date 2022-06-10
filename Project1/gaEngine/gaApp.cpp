#include "gaApp.h"

#include "../framework/fwScene.h"
#include "../framework/fwCamera.h"
#include "../framework/fwControl.h"

#include "Debug.h"
#include "gaBoundingBoxes.h"
#include "World.h"
#include "gaLevel.h"
#include "gaActor.h"
#include "gaBehavior.h"

#include "gaPlugin/gaPSounds.h"
#include "gaPlugin/gaPView.h"

#include "gaBehaviorNode/gaBehaviorDecorator.h"
#include "gaBehaviorNode/gaBehaviorLoop.h"
#include "gaBehaviorNode/gaBehaviorSequence.h"
#include "gaBehaviorNode/gaBFor.h"
#include "gaBehaviorNode/gaBehaviorSound.h"
#include "gaBehaviorNode/gaBNSatNav.h"
#include "gaBehaviorNode/gaMoveTo.h"
#include "gaBehaviorNode/gaBTurn.h"
#include "gaBehaviorNode/gaBSetVar.h"
#include "gaBehaviorNode/gaBCheckVar.h"
#include "gaBehaviorNode//gaBAlarm.h"

/**
 *
 */
GameEngine::App* GameEngine::App::m_currentApp = nullptr;
std::map<std::string, App*> GameEngine::App::m_apps;	// list of running app

/**
 * register darkforces entities for the flight recorder
 */
void GameEngine::App::registerFRclasses(void)
{
	g_Blackbox.registerClass("gaEntity", &gaEntity::create);
	g_Blackbox.registerClass("gaActor", &gaActor::create);
}

/**
 * init the BehaviorTree static plugins
 */
void GameEngine::App::registerBThandlers(void)
{
	GameEngine::Behavior::registerHandler("GameEngine:onViewPlayer", &GameEngine::Component::BehaviorTree::onViewPlayer);
	GameEngine::Behavior::registerHandler("GameEngine:onNotViewPlayer", &GameEngine::Component::BehaviorTree::onNotViewPlayer);
	GameEngine::Behavior::registerHandler("GameEngine:onHearSound", &GameEngine::Component::BehaviorTree::onHearSound);
	GameEngine::Behavior::registerHandler("GameEngine:onBulletHit", &GameEngine::Component::BehaviorTree::onBulletHit);

	GameEngine::Behavior::registerMessage("GameEngine:VIEW", gaMessage::Action::VIEW);
	GameEngine::Behavior::registerMessage("GameEngine:NOT_VIEW", gaMessage::Action::NOT_VIEW);
	GameEngine::Behavior::registerMessage("GameEngine:HEAR_SOUND", gaMessage::Action::HEAR_SOUND);
	GameEngine::Behavior::registerMessage("GameEngine:BULLET_HIT", gaMessage::Action::BULLET_HIT);
	GameEngine::Behavior::registerMessage("GameEngine:SatNav_CANCEL", gaMessage::Action::SatNav_CANCEL);
}

/**
 * register darkforces bevavionr nodes for the behavior engine
 */
void GameEngine::App::registerBTNodes(void)
{
	struct char_cmp {
		bool operator () (const char* a, const char* b) const
		{
			return strcmp(a, b) < 0;
		}
	};

	static const std::map<const char*, GameEngine::Behavior::createFunction, char_cmp> g_createNodes = {
		{"Decorator", GameEngine::Behavior::Decorator::create},
		{"Loop", GameEngine::Behavior::Loop::create},
		{"Sequence", GameEngine::Behavior::Sequence::create},
		{"GameEngine:For", GameEngine::Behavior::For::create},
		{"Sound", GameEngine::Behavior::Sound::create},
		{"SatNav", GameEngine::Behavior::SatNav::create},
		{"MoveTo", GameEngine::Behavior::MoveTo::create},
		{"GameEngine:Turn", GameEngine::Behavior::Turn::create},
		{"GameEngine:SetVar", GameEngine::Behavior::SetVar::create},
		{"GameEngine:CheckVar", GameEngine::Behavior::CheckVar::create},
		{"GameEngine:Alarm", GameEngine::Behavior::Alarm::create}
	};

	for (auto& b : g_createNodes) {
		GameEngine::Behavior::registerNode(b.first, b.second);
	}
}

#ifdef _DEBUG
/**
 * register messages & classed for debugger
 */
void GameEngine::App::registerDebugger(void)
{
	static const std::map<int32_t, const char*> g_definitions = {
		{ gaMessage::Action::COLLIDE, "COLLIDE"},
		{ gaMessage::Action::TIMER, "TIMER"},
		{ gaMessage::Action::DELETE_ENTITY, "DELETE_ENTITY"},
		{ gaMessage::Action::MOVE, "MOVE"},
		{ gaMessage::Action::CANT_MOVE, "CANT_MOVE"},
		{ gaMessage::Action::ROTATE, "ROTATE"},
		{ gaMessage::Action::REGISTER_SOUND, "REGISTER_SOUND"},
		{ gaMessage::Action::PLAY_SOUND, "PLAY_SOUND"},
		{ gaMessage::Action::PROPAGATE_SOUND, "PLAY_SOUND"},
		{ gaMessage::Action::STOP_SOUND, "STOP_SOUND"},
		{ gaMessage::Action::WANT_TO_MOVE, "WANT_TO_MOVE"},
		{ gaMessage::Action::COLLISION, "COLLISION"},
		{ gaMessage::Action::MOVE_TO, "MOVE_TO"},
		{ gaMessage::Action::WORLD_INSERT,"WORLD_INSERT" },
		{ gaMessage::Action::WORLD_REMOVE, "WORLD_REMOVE"},
		{ gaMessage::Action::WOULD_FALL, "WOULD_FALL"},
		{ gaMessage::Action::FALL, "FALL"},
		{ gaMessage::Action::CONTROLLER, "CONTROLLER"},
		{ gaMessage::Action::SAVE_WORLD, "SAVE_WORLD"},
		{ gaMessage::Action::KEY, "KEY"},
		{ gaMessage::Action::LOOK_AT, "LOOK_AT"},
		{ gaMessage::Action::MOVE_AT, "MOVE_AT"},
		{ gaMessage::Action::ACTIVATE, "ACTIVATE"},
		{ gaMessage::Action::HIDE, "HIDE"},
		{ gaMessage::Action::UNHIDE, "UNHIDE"},
		{ gaMessage::Action::START_MOVE, "START_MOVE"},
		{ gaMessage::Action::END_MOVE, "END_MOVE"},
		{ gaMessage::Action::SatNav_GOTO, "SatNav_GOTO"},
		{ gaMessage::Action::SatNav_NOGO, "SatNav_NOGO"},
		{ gaMessage::Action::SatNav_CANCEL, "SatNav_CANCEL"},
		{ gaMessage::Action::SatNav_REACHED, "SatNAV_REACHED"},
		{ gaMessage::Action::TICK, "TICK"},
		{ gaMessage::Action::ALARM, "ALARM"},
		{ gaMessage::Action::VIEW, "VIEW"},
		{ gaMessage::Action::NOT_VIEW, "NOT_VIEW"},
		{ gaMessage::Action::BULLET_HIT, "BULLERT_HIT"},
		{ gaMessage::Action::BULLET_MISS, "BULLET_MISS"},
		{ gaMessage::Action::HEAR_SOUND, "HEAR_SOUND"},
		{ gaMessage::Action::HEAR_STOP, "HEAR_STOP"},
		{ gaMessage::Action::VOLUME_TRANSPARENCY, "VolumeTransparency"},
		{ gaMessage::Action::ADD_ITEM,	"AddItem"},
		{ gaMessage::Action::DROP_ITEM, "DropItem"},
	};

	static const std::map<int32_t, const std::map<int32_t, const char*>>  g_definitions_values;

	gaMessage::declareMessages(g_definitions, g_definitions_values);
}
#endif

GameEngine::App::App(const std::string& name, int width, int height, const std::string& post_processing, const std::string& defines):
	fwApp(name, width, height, post_processing, defines)
{
	m_camera = new fwCamera(width, height);
	m_scene = new fwScene();
	g_gaWorld.scene(m_scene);

	registerFRclasses();	// register darkforces entities for the flight recorder
	registerBThandlers();	// init the BehaviorTree static plugins
	registerBTNodes();		// register darkforces bevavionr nodes for the behavior engine
#ifdef _DEBUG
	registerDebugger();	// register messages & classed for debugger
#endif

	// add the plugins
	g_gaWorld.registerPlugin(&g_gaSoundEngine);
	g_gaWorld.registerPlugin(&g_gaViewEngine);

#ifdef _DEBUG
	m_debugger = new GameEngine::Debug(this);
#endif

	// record the app
	m_apps[name] = this;
	m_currentApp = this;
}

/**
 *
 */
glTexture* GameEngine::App::draw(time_t delta, fwRenderer* renderer)
{
	//draw the bounding boxes
	g_gaBoundingBoxes.draw(m_scene);
	g_gaWorld.process(33);

	m_level->draw(m_player->superSector(), m_camera); 	// update visible objects

	return m_renderer->draw(m_camera, m_scene);;
}

/**
 * screen resize event
 */
void GameEngine::App::resize(int width, int height)
{
	m_camera->set_ratio(width, height);
	float ratio = (float)width / (float)height;

	// inform all entities of the new screen ration
	GameEngine::World::send("GameEngine:Core", "*", gaMessage::Action::SCREEN_RESIZE, ratio);
}

/**
 * save and replace the current controler
 */
void GameEngine::App::pushControl(fwControl* newControler)
{
	m_currentApp->_pushControl(newControler);
}

void GameEngine::App::_pushControl(fwControl* newControler)
{
	m_controlers.push(m_control);
	m_control = newControler;
}

/**
 * restore the previous controler
 */
fwControl* GameEngine::App::popControl(void)
{
	return m_currentApp->_popControl();
}

fwControl* GameEngine::App::_popControl(void)
{
	m_control = m_controlers.top();
	m_controlers.pop();
	return m_control;
}

/**
 *
 */
GameEngine::App::~App()
{
}
