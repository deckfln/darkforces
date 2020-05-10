#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "../framework/fwSprites.h"
#include "dfAtlasTexture.h"

class fwScene;

class dfSprites: public fwSprites
{
	std::vector<glm::vec3> m_positions;		// position of each sprite
	std::vector<glm::vec3> m_directions;	// direction of the virtual object
	std::vector<glm::ivec3> m_textureIndex;

	int m_toDisplay = 0;					// number of sprites to display
	bool updated = true;					// the attributes have been updated and need to be pushed to the GPU
	bool m_added = false;

public:
	dfSprites(int nbSprites, dfAtlasTexture *atlas);
	void add(glm::vec3 position, int textureID);
	void update(void);						// push changes to the GPU
	void add2scene(fwScene* scene);
	~dfSprites();
};