#include "fwapp.h"

#include <iostream>

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

static fwApp *currentApp = nullptr;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	currentApp->resizeEvent(width, height);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	currentApp->mouseButton(button, action);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	currentApp->mouseMove(xpos, ypos);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	currentApp->mouseScroll(xoffset, yoffset);
}

/***
 *
 */
fwApp::fwApp(std::string name, int _width, int _height):
	height(_height),
	width(_width)
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(width, height, name.c_str() , NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// FRAME BUFFER
	colorMap = new glColorMap(width, height);
	glTexture *tex = colorMap->getColorTexture();
	source = new Uniform("screenTexture", tex);
	postProcessing = new fwPostProcessing("shaders/screen_vertex.glsl", "shaders/screen_fragment.glsl", source);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void fwApp::bindControl(fwOrbitControl *_control)
{
	control = _control;
}

void fwApp::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void fwApp::resizeEvent(int _width, int _height)
{
	width = _width;
	height = _height;

	resize(width, height);
}

void fwApp::mouseButton(int button, int action)
{
	if (control)
		control->mouseButton(button, action);
}

void fwApp::mouseMove(double xpos, double ypos)
{
	float x = xpos / width;
	float y = ypos / height;

	if (control)
		control->mouseMove(x, y);
}

void fwApp::mouseScroll(double xoffset, double yoffset)
{
	if (control)
		control->mouseScroll(xoffset, yoffset);
}

/***
 *
 */
void fwApp::run(void)
{
	currentApp = this;

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// 1st pass : shadows

		// 2nd pass : render to color buffer
		colorMap->bind();
		colorMap->clear();

		draw();
		
		colorMap->unbind();

		// 3rd pass : postprocessing
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		postProcessing->draw();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

/***
 *
 */
fwApp::~fwApp()
{
	delete colorMap;
	delete postProcessing;
	delete source;

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}
