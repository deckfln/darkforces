#pragma once
#include <list>
#include "../glEngine/glProgram.h"
#include "../glEngine/glUniformBuffer.h"

#include "fwObject3D.h"
#include "fwMesh.h"
#include "fwLight.h"
#include "fwCamera.h"
#include "materials/fwOutlineMaterial.h"
#include "fwSkybox.h"
#include "../glEngine/glColorMap.h"

class fwScene : public fwObject3D
{
	std::list <fwLight*> lights;

	fwSkybox *m_pBackground = nullptr;
	glm::vec3 *outlinecolor = nullptr;

public:
	fwScene();
	fwScene &addLight(fwLight *light);
	fwScene &setOutline(glm::vec3 *_color);
	std::list <fwLight*> get_lights(void) { return lights;  }

	fwScene &background(fwSkybox *_background) { m_pBackground = _background; return *this; };
	fwSkybox *background(void) { return m_pBackground; };

	~fwScene();
};
