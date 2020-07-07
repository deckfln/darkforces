#pragma once

#include <list>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include "../fwRenderer.h"

class fwRendererForward: public fwRenderer
{
private:
	void drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program, const std::string &defines);
	void setOutline(glm::vec4* _color);

public:
	fwRendererForward(int width, int height);
	glTexture *draw(fwCamera* camera, fwScene* scene);
	glTexture* getColorTexture(void);

	~fwRendererForward();
};