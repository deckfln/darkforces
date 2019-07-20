#pragma once

#include <list>

#include "../glEngine/glProgram.h"
#include "fwCamera.h"
#include "fwScene.h"

class fwRenderer
{
protected:
	std::map <std::string, glProgram*> m_programs;
	std::map <int, fwMaterial*> m_materials;
	void getAllChildren(fwObject3D* root, std::list <fwMesh*>& meshes, std::list <fwMesh*>& instances);

public:
	fwRenderer();

	virtual glTexture* draw(fwCamera* camera, fwScene* scene) { return nullptr; };
	virtual void start(void) {};
	virtual void stop(void) {};

	~fwRenderer();
};