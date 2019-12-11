#pragma once

#include <list>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include "../../glEngine/glColorMap.h"
#include "../fwCamera.h"
#include "../fwScene.h"
#include "../fwRenderer.h"

class fwRendererForward: public fwRenderer
{
private:
	void allChildren(fwObject3D* root, std::list <fwMesh*>& meshes, std::list <fwMesh*>& instances);
	void drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program, std::string &defines);
	void setOutline(glm::vec4* _color);

public:
	fwRendererForward(int width, int height);
	glTexture *draw(fwCamera* camera, fwScene* scene);
	glTexture* getColorTexture(void);
	glm::vec2 size(void);

	~fwRendererForward();
};