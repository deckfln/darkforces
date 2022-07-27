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

namespace Framework {
	class Mesh2D;
}

class fwScene : public fwObject3D
{
	std::list <fwLight*> m_lights;

	fwBackground *m_pBackground = nullptr;
	std::vector<fwHUD*> m_uis;						// stack of flat UI to display with overdraw
	std::list<Framework::Mesh2D*> m_meshes2D;		// 2D meshes to draw on top of the 3D meshes

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
	inline std::list<Framework::Mesh2D*>& meshes2D(void) { return m_meshes2D; };

	void hud(fwHUD* hud);							// Add a new flatUI on top of the previous one
	bool hasUI(void);								// if there are UI's to draw
	void drawUI(void);								// Draw the UI's
	void addMesh2D(Framework::Mesh2D*);				// add a 2D element

#ifdef _DEBUG
	void debugGUI(bool debug);
#endif

};
