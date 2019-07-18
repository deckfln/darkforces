#pragma once

#include "fwCamera.h"
#include "fwScene.h"

class fwRenderer
{
public:
	fwRenderer();

	virtual glTexture* draw(fwCamera* camera, fwScene* scene) { return nullptr; };
	virtual void start(void) {};
	virtual void stop(void) {};

	~fwRenderer();
};