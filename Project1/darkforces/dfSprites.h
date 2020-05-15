#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <map>
#include <string>

#include "../framework/fwSprites.h"
#include "dfAtlasTexture.h"

#include "dfModel.h"

class fwScene;
class dfObject;

class dfSprites: public fwSprites
{
	std::vector<glm::vec3> m_positions;		// position of each sprite
	std::vector<glm::vec3> m_directions;	// direction of the virtual object
	std::vector<glm::vec3> m_textureIndex;

	int m_nbObjects= 0;
	std::vector<dfObject *> m_objects;

	int m_nbModels = 0;
	std::vector<SpriteModel> m_models;
	std::map<std::string, int> m_modelsIndex;	// name of the model to index in the m_models table

	int m_toDisplay = 0;					// number of sprites to display
	bool updated = true;					// the attributes have been updated and need to be pushed to the GPU
	bool m_added = false;

public:
	dfSprites(int nbSprites, dfAtlasTexture *atlas);
	void addModel(dfModel *model);
	void add(dfObject *object);
	void update(time_t t);						// push changes to the GPU
	void add2scene(fwScene* scene);
	~dfSprites();
};