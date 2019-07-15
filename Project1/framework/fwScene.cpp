#include "fwScene.h"
#include <iterator> 
#include <map>
#include <list>
#include <string>


fwScene::fwScene()
{
}

fwScene& fwScene::setOutline(glm::vec3* _color)
{
	outlinecolor = _color;
	return *this;
}

fwScene &fwScene::addLight(fwLight *light)
{
	addChild(light);

	lights.push_back(light);
	return *this;
}

fwScene::~fwScene()
{
}
