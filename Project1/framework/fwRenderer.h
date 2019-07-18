#pragma once

#include <list>

#include "fwCamera.h"
#include "fwScene.h"

class fwRenderer
{
protected:
	void getAllChildren(fwObject3D* root, std::list <fwMesh*>& meshes, std::list <fwMesh*>& instances);

public:
	fwRenderer();

	virtual glTexture* draw(fwCamera* camera, fwScene* scene) { return nullptr; };
	virtual void start(void) {};
	virtual void stop(void) {};

	~fwRenderer();
};