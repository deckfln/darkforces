#pragma once
#include <list>
#include "../glEngine/glProgram.h"
#include "../glEngine/glUniformBuffer.h"

#include "fwObject3D.h"
#include "framework/Mesh.h"
#include "framework/Light.h"
#include "framework/Camera.h"
#include "materials/OutlineMaterial.h"

class fwScene : public fwObject3D
{
	std::map <std::string, glProgram *> programs;
	std::map <int, Material *> materials;

	OutlineMaterial *outline_material = nullptr;
	glProgram *outline_program = nullptr;

	Light *lights[10];
	int current_light;

	void parseChildren(fwObject3D *root, std::map<std::string, std::map<int, std::list <Mesh *>>> &meshesPerMaterial, std::string &codeLights, std::string &defines);

public:
	fwScene();
	fwScene &addLight(Light *light);
	fwScene &setOutline(glm::vec4 *_color);
	void draw(Camera *camera);
	~fwScene();
};
