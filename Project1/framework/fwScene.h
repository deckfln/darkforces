#pragma once
#include <list>
#include <glm/vec3.hpp>

#include "../glEngine/glProgram.h"
#include "../glEngine/glUniformBuffer.h"

#include "fwObject3D.h"

class fwLight;
class fwBackground;
class fwHUD;
class fwHUDelement;

class fwScene : public fwObject3D
{
	std::list <fwLight*> m_lights;

	fwBackground *m_pBackground = nullptr;
	fwHUD* m_hud = nullptr;

	glm::vec3 *outlinecolor = nullptr;

public:
	fwScene();
	fwScene &addLight(fwLight *light);
	fwScene &setOutline(glm::vec3 *_color);
	const std::list <fwLight*>& lights(void) { return m_lights;  }

	fwScene &background(fwBackground *_background) { m_pBackground = _background; return *this; };
	fwBackground *background(void) { return m_pBackground; };

	fwScene& hud(fwHUDelement* element);
	fwHUD* hud(void) { return m_hud; };

	~fwScene();
};
