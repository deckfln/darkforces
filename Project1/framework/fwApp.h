#pragma once

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../glEngine/glColorMap.h"
#include "fwPostProcessing.h"
#include "controls/fwOrbitControl.h"
#include "render/fwForwardRenderer.h"

class fwApp
{
	int height;
	int width;
	glm::vec2 m_pixelsize;
	GLFWwindow* window = nullptr;
	glColorMap *colorMap = nullptr;
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
	virtual void draw(fwForwardRenderer *renderer) {};
	virtual void resize(int x, int y) {};
	~fwApp();
};