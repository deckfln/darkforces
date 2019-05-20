#pragma once
#include <list>
#include "../glEngine/glProgram.h"
#include "../glEngine/glUniformBuffer.h"

#include "fwObject3D.h"
#include "framework/fwMesh.h"
#include "framework/Light.h"
#include "framework/Camera.h"
#include "materials/fwOutlineMaterial.h"

class fwScene : public fwObject3D
{
	std::map <std::string, glProgram *> programs;
	std::map <int, fwMaterial *> materials;

	fwOutlineMaterial *outline_material = nullptr;

	Light *lights[10];
	int current_light;

	void parseChildren(fwObject3D *root, std::map<std::string, std::map<int, std::list <fwMesh *>>> &meshesPerMaterial, std::string &codeLights, std::string &defines);

public:
	fwScene();
	fwScene &addLight(Light *light);
	fwScene &setOutline(glm::vec4 *_color);
	void draw(Camera *camera);
	~fwScene();
};
