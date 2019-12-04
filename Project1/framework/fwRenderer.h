#pragma once

#include <list>
#include <vector>

#include "../glEngine/glProgram.h"
#include "fwCamera.h"
#include "fwScene.h"

// list of possible meshes
enum {
	NORMAL,
	INSTANCED,
	SKINNED
};

class fwRenderer
{
protected:
	glColorMap* m_colorMap = nullptr;

	std::map <std::string, glProgram*> m_programs;
	std::map <int, fwMaterial*> m_materials;
	void getAllChildren(fwObject3D* root, std::vector<std::list <fwMesh*>>& meshes);
	bool drawShadows(fwCamera* camera, fwScene* scene);

public:
	fwRenderer();

	virtual glTexture* draw(fwCamera* camera, fwScene* scene) { return nullptr; };
	void start(void);
	void stop(void);
	glm::vec2 size(void);

	~fwRenderer();
};