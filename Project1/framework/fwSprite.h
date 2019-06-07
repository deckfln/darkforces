#pragma once

#include "fwMesh.h"

#define SPRITE 4

class fwSprite : public fwMesh
{
	int m_size = 5;
public:
	fwSprite(glm::vec3 *positions, int nb, fwTexture *texture);
	void draw(glProgram *);
	~fwSprite();
};
