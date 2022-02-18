#pragma once
#include <list>
#include <glm/vec3.hpp>
#include <string>
#include <map>

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

	// debugger
	std::map<fwObject3D*, bool> m_inspector;

public:
	fwScene();
	fwScene &addLight(fwLight *light);
	fwScene &setOutline(glm::vec3 *_color);
	inline const std::list <fwLight*>& lights(void) { return m_lights;  }

	inline fwScene &background(fwBackground *_background) { m_pBackground = _background; return *this; };
	inline fwBackground *background(void) { return m_pBackground; };

	inline void hud(fwHUD* hud) { m_hud = hud; };
	fwScene& hud(fwHUDelement* element);
	inline fwHUD* hud(void) { return m_hud; };

	void debugGUI(void);

	~fwScene();
};
