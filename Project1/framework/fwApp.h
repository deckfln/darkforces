#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "glEngine/glColorMap.h"
#include "framework/fwPostProcessing.h"
#include "framework/controls/fwOrbitControl.h"

class fwApp
{
	int height;
	int width;
	GLFWwindow* window = nullptr;
	glColorMap *colorMap = nullptr;
	fwPostProcessing *postProcessing = nullptr;
	fwOrbitControl *control = nullptr;
	Uniform *source = nullptr;

	void processInput(GLFWwindow *window);

public:
	fwApp(std::string name, int _width, int _height);

	void mouseButton(int button, int action);
	void mouseMove(double xpos, double ypos);
	void mouseScroll(double xpos, double ypos);
	void resizeEvent(int _width, int _height);

	void run(void);
	void bindControl(fwOrbitControl *control);
	virtual void draw(void) {};
	virtual void resize(int x, int y) {};
	~fwApp();
};