#pragma once

#include <list>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include "../../glEngine/glGBuffer.h"
#include "../fwCamera.h"
#include "../fwScene.h"
#include "../fwRenderer.h"

class fwRendererDefered: public fwRenderer
{
	glGBuffer* m_colorMap = nullptr;
private:
	void drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program);

	void parseChildren(fwObject3D* root,
		std::map<std::string, std::map<int, std::list <fwMesh*>>>& opaqueMeshPerMaterial,
		std::list <fwMesh*>& transparentMeshes,
		fwCamera* camera);

public:
	fwRendererDefered(int width, int height);
	glTexture* draw(fwCamera* camera, fwScene* scene);
	void start(void);
	void stop(void);
	glm::vec2 size(void);
	glTexture* getColorTexture(void);

	~fwRendererDefered();
};