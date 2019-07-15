#pragma once

#include <list>
#include <map>
#include <string>
#include <glm/glm.hpp>

#include "../../glEngine/glColorMap.h"
#include "../fwCamera.h"
#include "../fwScene.h"

class fwForwardRenderer
{
	glColorMap *colorMap = nullptr;
	std::map <std::string, glProgram*> programs;
	std::map <int, fwMaterial*> materials;

private:
	void allChildren(fwObject3D* root, std::list <fwMesh*>& meshes, std::list <fwMesh*>& instances);
	void parseChildren(fwObject3D* root,
		std::map<std::string, std::map<int, std::list <fwMesh*>>>& opaqueMeshPerMaterial,
		std::list <fwMesh*>& transparentMeshes,
		std::string& codeLights,
		std::string& defines,
		bool withShadow,
		fwCamera* camera);
	void drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program, std::string defines);
	void setOutline(glm::vec4* _color);

public:
	fwForwardRenderer(int width, int height);
	void draw(fwCamera* camera, fwScene* scene);
	glTexture* getColorTexture(void);
	void start(void);
	void stop(void);

	~fwForwardRenderer();
};