#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../glEngine/glColorMap.h"
#include "fwPostProcessing.h"
#include "fwControl.h"
#include "fwRenderer.h"

class fwApp
{
protected:
	int height = 0;
	int width = 0;
	glm::vec2 m_pixelsize = glm::vec2(0);
	GLFWwindow* window = nullptr;
	fwPostProcessing *postProcessing = nullptr;
	fwControl *m_control = nullptr;
	fwUniform *source = nullptr;
	fwRenderer* m_renderer = nullptr;

	void processInput(GLFWwindow *window);

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

	~fwApp();
};