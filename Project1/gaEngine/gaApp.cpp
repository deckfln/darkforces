#include "gaApp.h"

#include "../framework/fwScene.h"
#include "../framework/fwCamera.h"

#include "Debug.h"
#include "gaBoundingBoxes.h"
#include "World.h"
#include "gaLevel.h"
#include "gaActor.h"

#include "gaPlugin/gaPSounds.h"

/**
 *
 */
GameEngine::App::App(const std::string& name, int width, int height, const std::string& post_processing, const std::string& defines):
	fwApp(name, width, height, post_processing, defines)
{
	m_camera = new fwCamera(width, height);
	m_scene = new fwScene();
	g_gaWorld.scene(m_scene);

	// prepare the flight recorder
	g_Blackbox.registerClass("gaEntity", &gaEntity::create);
	g_Blackbox.registerClass("gaActor", &gaActor::create);

	// add the plugins
	g_gaWorld.registerPlugin(&g_gaSoundEngine);

#ifdef _DEBUG
	m_debugger = new GameEngine::Debug(this);
#endif
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
 *
 */
void GameEngine::App::resize(int width, int height)
{
	m_camera->set_ratio(width, height);
}

/**
 *
 */
GameEngine::App::~App()
{
}
