#pragma once

#include <time.h>

#include "../framework/fwApp.h"
#ifdef _DEBUG
#include "Debug.h"
#endif

class glTexture;
class fwMesh;
class fwRenderer;
class gaActor;

namespace GameEngine {
	class Level;

	class App : public fwApp {
	protected:
		fwMesh* m_fwCamera = nullptr;
		Level *m_level = nullptr;
		gaActor* m_player = nullptr;

		void registerFRclasses(void);	// register darkforces entities for the flight recorder
		void registerBThandlers(void);	// init the BehaviorTree static plugins
		void registerBTNodes(void);		// register darkforces bevavionr nodes for the behavior engine

#ifdef _DEBUG
		friend Debug;
		virtual void registerDebugger(void);	// register messages & classed for debugger
#endif

	public:
		App(const std::string& name, int width, int height, const std::string& post_processing, const std::string& defines);
		glTexture* draw(time_t delta, fwRenderer* renderer) override;
		void resize(int x, int y);
		~App();
	};
}