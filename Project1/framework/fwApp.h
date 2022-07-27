#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


#include "../glEngine/glColorMap.h"
#include "fwPostProcessing.h"
#include "fwControl.h"
#include "fwRenderer.h"

#ifdef _DEBUG
#include "fwDebug.h"
#include <imgui.h>
#endif

class fwApp
{
protected:
	fwCamera* m_camera = nullptr;
	fwScene* m_scene = nullptr;;

	int m_height = 0;
	int m_width = 0;
	glm::vec2 m_pixelsize = glm::vec2(0);
	GLFWwindow* window = nullptr;
	fwPostProcessing *postProcessing = nullptr;
	fwControl *m_control = nullptr;
	fwUniform *source = nullptr;
	fwRenderer* m_renderer = nullptr;

	friend Framework::Debug;

#ifdef _DEBUG
	Framework::Debug* m_debugger = nullptr;
	void virtual createDocks(ImGuiID dockspace_id);		// create the initial docking model
#endif

public:
	fwApp(std::string name, int _width, int _height, std::string post_processing, std::string defines);

	void mouseButton(int button, int action);
	void mouseMove(double xpos, double ypos);
	void mouseScroll(double xpos, double ypos);
	void resizeEvent(int _width, int _height);
	void keyEvent(int key, int scancode, int action, int mods);

	void run(void);
	void bindControl(fwControl *control);
	virtual glTexture* draw(time_t delta, fwRenderer *renderer) { return nullptr; };
	virtual void resize(int x, int y) {};
	virtual void keypress(void) {};
	virtual void renderGUI(void);
	virtual void processInput(void);

	~fwApp();
};