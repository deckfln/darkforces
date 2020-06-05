#pragma once

#include <list>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include "../../glEngine/glGBuffer.h"
#include "../fwCamera.h"
#include "../fwScene.h"
#include "../fwRenderer.h"
#include "../postprocessing/fwPostProcessingBloom.h"

class fwRendererDefered: public fwRenderer
{
	//glGBuffer* m_colorMap = nullptr;
	fwPostProcessingBloom* m_bloom = nullptr;

	// final rendering target
	glColorMap* m_lightRendering = nullptr;
	fwGeometry* m_renderGeometry = nullptr;
	glVertexArray* m_quad = nullptr;

private:
	void buildDeferedShader(std::list <fwMesh*>& meshes, fwCamera* camera, std::map<std::string, std::map<int, std::list <fwMesh*>>>& meshPerMaterial);
	void drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program, std::string &defines);
	void drawMeshes(std::list <fwMesh*>& meshesPerMaterial, fwCamera* camera);
	void mergeMTR(fwScene *scene);

public:
	fwRendererDefered(int width, int height, bool withBloom=false);
	glTexture* draw(fwCamera* camera, fwScene* scene);
	glm::vec2 size(void);
	glTexture* getColorTexture(void);

	~fwRendererDefered();
};