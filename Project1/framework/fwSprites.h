#pragma once

#include "fwMesh.h"

class fwSprites : public fwMesh
{
protected:
	int m_size = 5;

public:
	fwSprites(int size=0);
	fwSprites(glm::vec3* positions, int nb, fwTexture* texture, float radius);
	void set(glm::vec3* positions, fwTexture* texture, float radius);
	void draw(glProgram *);
	~fwSprites();
};
