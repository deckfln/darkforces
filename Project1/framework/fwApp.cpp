#include "fwapp.h"

#include <iostream>

#include "fwRenderer.h"
#include "render/fwRendererForward.h"
#include "render/fwRendererDefered.h"

const int caped_fps = FALSE;

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

static fwApp *currentApp = nullptr;
static fwRenderer* renderer = nullptr;

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

	renderer = new fwRendererDefered(SCR_WIDTH, SCR_HEIGHT);

	// post processing buffer
	source = new fwUniform("screenTexture", (glTexture *)nullptr);

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

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		this->keypress();
}

void fwApp::resizeEvent(int _width, int _height)
{
	width = _width;
	height = _height;

	glm::vec2 cm = renderer->size();
	m_pixelsize.x = 1.0 / cm.x;
	m_pixelsize.y = 1.0 / cm.y;

	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	resize(width, height);
	glViewport(0, 0, width, height);

	if (control) {
		control->update();
	}
}

void fwApp::mouseButton(int button, int action)
{
	if (control)
		control->mouseButton(button, action);
}

void fwApp::mouseMove(double xpos, double ypos)
{
	double x = xpos / width;
	double y = ypos / height;

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

	time_t current = GetTickCount64();
	time_t start = GetTickCount64();
	time_t next = current + 33;
	time_t elapsed = 0;
	time_t wait = 0;
	time_t now;
	time_t time_budget = 33;
	time_t last_frame_time = 33;

	std::string s;

	int fps = 0;

	while (!glfwWindowShouldClose(window))
	{
		current = GetTickCount64();

#ifdef BENCHMARK
		if (current - start > 300000) {
			break;
		}
#endif

		fps++;
		if ((fps % 3) == 0) {
			time_budget++;	// compensate for the 33.33333ms that we round to 33ms
		}

		// input
		// -----
		processInput(window);

		glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		//glFrontFace(GL_CCW);

		// 2nd pass : render to color buffer
		renderer->start();

		glTexture *color = draw(last_frame_time, renderer);
		
		renderer->stop();

		// 3rd pass : postprocessing
		//glCullFace(GL_BACK);
		postProcessing->draw(color);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (caped_fps) {
			now = GetTickCount64();
			elapsed = now - current;

			if (elapsed < time_budget) {
				wait = time_budget - elapsed;
				Sleep(wait);
				last_frame_time = time_budget;
				time_budget = 33;
				next = current + time_budget;
				// s += "short " + std::to_string(elapsed) + " " + std::to_string(wait) + " " + std::to_string(current)  +">>"+ std::to_string(now) +">>"+ std::to_string(next) + "\n";
			}
			else {
				time_budget = 33 * ((elapsed / 33) + 1);
				next = current + time_budget;
				time_budget = next - now;
				last_frame_time = elapsed;
				//s += "long " + std::to_string(elapsed) + " " + std::to_string(time_budget) + " " + std::to_string(current) + ">>" + std::to_string(now) + ">>" + std::to_string(next) + "\n";
			}
		}
		else {
			now = GetTickCount64();
			last_frame_time = now - current;
		}
	}

	std::cout << "Frames " << fps << std::endl;
}

/***
 *
 */
fwApp::~fwApp()
{
	delete renderer;
	delete postProcessing;
	delete source;

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}
