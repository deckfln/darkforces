#pragma once
#include <list>
#include "../glEngine/glProgram.h"
#include "../glEngine/glUniformBuffer.h"

#include "fwObject3D.h"
#include "fwMesh.h"
#include "fwLight.h"
#include "framework/fwCamera.h"
#include "materials/fwOutlineMaterial.h"
#include "fwSkybox.h"
#include "../glEngine/glColorMap.h"

class fwScene : public fwObject3D
{
	std::map <std::string, glProgram *> programs;
	std::map <int, fwMaterial *> materials;

	fwOutlineMaterial *outline_material = nullptr;

	fwLight *lights[10];
	int current_light;

	fwSkybox *m_pBackground = nullptr;

	void allChildren(fwObject3D *root, std::list <fwMesh *> &meshes, std::list <fwMesh *> &instances);
	void parseChildren(fwObject3D *root,
		std::map<std::string, std::map<int, std::list <fwMesh *>>> &opaqueMeshPerMaterial,
		std::list <fwMesh *> &transparentMeshes,
		std::string &codeLights,
		std::string &defines,
		bool withShadow,
		fwCamera *camera);
	void drawMesh(fwCamera *camera, fwMesh *mesh, glProgram *glProgram, std::string defines);

public:
	fwScene();
	fwScene &addLight(fwLight *light);
	fwScene &setOutline(glm::vec4 *_color);

	fwScene &background(fwSkybox *_background) { m_pBackground = _background; return *this; };

	void draw(fwCamera *camera, glColorMap *colorMap);
	~fwScene();
};
