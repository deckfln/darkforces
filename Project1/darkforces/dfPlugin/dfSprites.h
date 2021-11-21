#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <map>
#include <string>
#include <queue>

#include "../../framework/fwSprites.h"

#include "../../gaEngine/gaPlugin.h"

#include "../dfAtlasTexture.h"
#include "../dfModel.h"
#include "../dfComponent/dfCSprite.h"

class fwScene;
class dfSprite;

namespace DarkForces {
	namespace Plugins {
		class dfSprites : public GameEngine::Plugin, fwSprites
		{
			std::vector<glm::vec3> m_positions;						// position of each sprite
			std::vector<glm::vec3> m_directions;					// direction of the virtual object
			std::vector<glm::vec4> m_textureIndex;					// state & frame for the GPU to display the correct texture

			int m_nbObjects = 0;
			std::vector<void*> m_objects;							// list of objects (may include NULL when released)
			std::vector<bool> m_newSprite;
			std::queue<int> m_freeList;								// Queue of available slots in m_objects

			int m_nbModels = 0;
			bool m_dirtyModels = true;
			GLmodel m_models;

			std::map<std::string, int> m_modelsIndex;				// name of the model to index in the m_models table

			int m_toDisplay = 0;									// number of sprites to display
			bool m_updated = true;									// the attributes have been updated and need to be pushed to the GPU
			bool m_added = false;

		public:
			dfSprites(void);
			dfSprites(int nbSprites, dfAtlasTexture* atlas);
			void init(int nbSprites, dfAtlasTexture* atlas);		// initialise all data for the sprites manager
			void addModel(dfModel* model);
			void add(DarkForces::Component::Sprite* object);		// add a sprite
			void update(void);										// push changes to the GPU
			void remove(DarkForces::Component::Sprite* object);		// remove a sprite

			void OnWorldInsert(void);								// trigger when inserted in a gaWorld
			void OnWorldRemove(void);								// trigger when from the gaWorld

			void afterProcessing(void) override;					// execute the plugin after processing the message queue
		};
	}
}

extern DarkForces::Plugins::dfSprites g_dfSpritesEngine;