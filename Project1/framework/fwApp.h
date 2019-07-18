#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../glEngine/glColorMap.h"
#include "fwPostProcessing.h"
#include "controls/fwOrbitControl.h"
#include "fwRenderer.h"

class fwApp
{
	int height = 0;
	int width = 0;
	glm::vec2 m_pixelsize = glm::vec2(0);
	GLFWwindow* window = nullptr;
	fwPostProcessing *postProcessing = nullptr;
	fwOrbitControl *control = nullptr;
	fwUniform *source = nullptr;
	
	void processInput(GLFWwindow *window);

public:
	fwApp(std::string name, int _width, int _height, std::string post_processing, std::string defines);

	void mouseButton(int button, int action);
	void mouseMove(double xpos, double ypos);
	void mouseScroll(double xpos, double ypos);
	void resizeEvent(int _width, int _height);

	void run(void);
	void bindControl(fwOrbitControl *control);
	virtual glTexture* draw(fwRenderer *renderer) { return nullptr; };
	virtual void resize(int x, int y) {};
	~fwApp();
};