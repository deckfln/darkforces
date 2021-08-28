#include "fwapp.h"

#include <iostream>
#include "../config.h"

#include "fwScene.h"
#include "fwRenderer.h"
#include "render/fwRendererForward.h"
#include "render/fwRendererDefered.h"

const int caped_fps = TRUE;

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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	currentApp->keyEvent(key, scancode, action, mods);
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

#ifdef _DEBUG
	// Setup Dear ImGui context
	const char* glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
#endif

	// setup the backend render
	glfwMakeContextCurrent(window);

	m_renderer = new fwRendererDefered(SCR_WIDTH, SCR_HEIGHT);

	// post processing buffer
	source = new fwUniform("screenTexture", (glTexture *)nullptr);

	fwUniform *pixelsize = new fwUniform("pixelsize", &m_pixelsize);

	postProcessing = new fwPostProcessing(post_processing + "/vertex.glsl", post_processing + "/fragment.glsl", source, defines);
	postProcessing->addUniform(pixelsize);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
}

void fwApp::bindControl(fwControl *_control)
{
	m_control = _control;
}

/**
 * Render the debugger
 */
void fwApp::renderGUI(void)
{
#ifdef _DEBUG
	if (m_debugger)
		m_debugger->render();
#endif
}

void fwApp::processInput(void)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void fwApp::resizeEvent(int _width, int _height)
{
	width = _width;
	height = _height;

	glm::vec2 cm = m_renderer->size();
	m_pixelsize.x = 1.0f / cm.x;
	m_pixelsize.y = 1.0f / cm.y;

	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	resize(width, height);
	glViewport(0, 0, width, height);
}

void fwApp::mouseButton(int button, int action)
{
	if (m_control)
		m_control->mouseButton(button, action);
}

void fwApp::mouseMove(double xpos, double ypos)
{
	double x = xpos / width;
	double y = ypos / height;

	if (m_control)
		m_control->mouseMove(x, y);
}

void fwApp::mouseScroll(double xoffset, double yoffset)
{
	if (m_control)
		m_control->mouseScroll(xoffset, yoffset);
}

void fwApp::keyEvent(int key, int scancode, int action, int mods)
{
	if (m_control)
		m_control->keyEvent(key, scancode, action, mods);
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
	bool show_demo_window = true;

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

		glfwPollEvents();

		// input
		// -----
		processInput();
		if (m_control)
			m_control->update(last_frame_time);	// let the controller update itself if needed

		// todo this trigger an error in nsigh
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		//glFrontFace(GL_CCW);

		// 2nd pass : render to color buffer
		m_renderer->start();
		glTexture *color = draw(last_frame_time, m_renderer);
		m_renderer->stop();

		// 3rd pass : post-processing
		//glCullFace(GL_BACK);
		postProcessing->draw(color);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------

#ifdef _DEBUG
		// Start the Dear ImGui frame
		// Update and Render additional Platform Windows
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// render your GUI
		if (m_debugger)
			m_debugger->render();

		// Render dear imgui into screen
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#endif
		glfwSwapBuffers(window);

		// render the game

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

#ifdef _DEBUG
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
#endif


	std::cout << "Frames " << fps << std::endl;
}

/***
 *
 */
fwApp::~fwApp()
{
	delete m_renderer;
	delete postProcessing;
	delete source;
	//delete m_control;
	delete m_scene;
	delete m_camera;

	glfwDestroyWindow(window);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}
