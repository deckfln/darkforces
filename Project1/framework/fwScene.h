#pragma once
#include <list>
#include <glm/vec3.hpp>

#include "../glEngine/glProgram.h"
#include "../glEngine/glUniformBuffer.h"

#include "fwObject3D.h"
#include "fwCamera.h"
#include "materials/fwOutlineMaterial.h"
#include "../glEngine/glColorMap.h"

class fwLight;
class fwBackground;

class fwScene : public fwObject3D
{
	std::list <fwLight*> lights;

	fwBackground *m_pBackground = nullptr;
	glm::vec3 *outlinecolor = nullptr;

public:
	fwScene();
	fwScene &addLight(fwLight *light);
	fwScene &setOutline(glm::vec3 *_color);
	std::list <fwLight*> get_lights(void) { return lights;  }

	fwScene &background(fwBackground *_background) { m_pBackground = _background; return *this; };
	fwBackground *background(void) { return m_pBackground; };

	~fwScene();
};
