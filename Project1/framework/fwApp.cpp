#include "fwapp.h"

#include "render/fwForwardRenderer.h"
#include <iostream>

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

static fwApp *currentApp = nullptr;
static fwForwardRenderer* renderer = nullptr;

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
fwApp::fwApp(std::string name, int _width, int _height, std::string post_processing, std::string defines):
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

	renderer = new fwForwardRenderer(SCR_WIDTH, SCR_HEIGHT);

	// post processing buffer
	glTexture *tex = renderer->getColorTexture();
	source = new fwUniform("screenTexture", tex);

	m_pixelsize.x = 1.0 / (width*2.0);
	m_pixelsize.y = 1.0 / (height*2.0);
	fwUniform *pixelsize = new fwUniform("pixelsize", &m_pixelsize);

	postProcessing = new fwPostProcessing(post_processing + "/vertex.glsl", post_processing + "/fragment.glsl", source, defines);
	postProcessing->addUniform(pixelsize);

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

	glm::vec2 cm = colorMap->size();
	m_pixelsize.x = 1.0 / cm.x;
	m_pixelsize.y = 1.0 / cm.y;

	SCR_WIDTH = width;
	SCR_HEIGHT = height;
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

		glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		//glFrontFace(GL_CCW);

		// 2nd pass : render to color buffer
		renderer->start();

		draw(renderer);
		
		renderer->stop();

		// 3rd pass : postprocessing
		//glCullFace(GL_BACK);
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
