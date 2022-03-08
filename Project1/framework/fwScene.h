#pragma once
#include <list>
#include <glm/vec3.hpp>
#include <string>
#include <map>
#include <vector>

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
	std::vector<fwHUD*> m_uis;		// stack of flat UI to display with overdraw

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

	void hud(fwHUD* hud);	// Add a new flatUI on top of the previous one
	bool hasUI(void);		// if there are UI's to draw
	void drawUI(void);		// Draw the UI's

#ifdef _DEBUG
	void debugGUI(void);
#endif

	~fwScene();
};
