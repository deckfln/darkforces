#pragma once
#include <list>
#include "../glEngine/glProgram.h"
#include "../glEngine/glUniformBuffer.h"

#include "fwObject3D.h"
#include "fwMesh.h"
#include "fwLight.h"
#include "framework/fwCamera.h"
#include "materials/fwOutlineMaterial.h"

class fwScene : public fwObject3D
{
	std::map <std::string, glProgram *> programs;
	std::map <int, fwMaterial *> materials;

	fwOutlineMaterial *outline_material = nullptr;

	fwLight *lights[10];
	int current_light;

	void allChildren(fwObject3D *root, std::list <fwMesh *> &meshes, std::list <fwMesh *> &instances);
	void parseChildren(fwObject3D *root, std::map<std::string, std::map<int, std::list <fwMesh *>>> &meshesPerMaterial, std::string &codeLights, std::string &defines, bool withShadow);

public:
	fwScene();
	fwScene &addLight(fwLight *light);
	fwScene &setOutline(glm::vec4 *_color);
	void draw(fwCamera *camera);
	~fwScene();
};
