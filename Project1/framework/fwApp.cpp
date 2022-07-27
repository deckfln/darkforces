#include "fwapp.h"

#include <direct.h>
#include <limits.h>

#ifdef _DEBUG
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#endif

#include <iostream>
#include <chrono>
#include <thread>
using namespace std::literals::chrono_literals;

#include "../config.h"

#include "../include/imnodes.h"

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

/**
 * set the root folder
 */
static void rootfolder(void)
{
	char tmp[1024];
	if (_getcwd(tmp, 1024) == nullptr) {
		std::cout << "FrameWork::rootfolder can't read current path" << std::endl;
		exit(-1);
	}

	std::string path(tmp);
	int hasInclude = path.find("Release");
	if (hasInclude >= 0) {
		path.replace(hasInclude, 7, "Project1");
	}
	hasInclude = path.find("Debug");
	if (hasInclude >= 0) {
		path.replace(hasInclude, 5, "Project1");
	}

	const char* c = path.c_str();
	if (_chdir(c) != 0) {
		std::cout << "FrameWork::rootfolder can't set current path " << path << std::endl;
		exit(-1);
	}
}

/**
 * create the initial docking model
 */
void fwApp::createDocks(ImGuiID dockspace_id)
{
	// split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
	//   window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
	//   out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
	auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
	auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, nullptr, &dockspace_id);

	// we now dock our windows into the docking node we made above
	ImGui::DockBuilderDockWindow("3D View", dockspace_id);
	ImGui::DockBuilderDockWindow("Explorer", dock_id_left);
	ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
}

/***
 *
 */
fwApp::fwApp(std::string name, int _width, int _height, std::string post_processing, std::string defines):
	m_height(_height),
	m_width(_width)
{
	// set the root app folder
	rootfolder();

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
	window = glfwCreateWindow(m_width, m_height, name.c_str() , NULL, NULL);
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

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

#ifdef _DEBUG
	// Setup Dear ImGui context
	const char* glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImNodes::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	// 
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// setup the backend render
	//glfwMakeContextCurrent(window);
#endif

	m_renderer = new fwRendererDefered(SCR_WIDTH, SCR_HEIGHT);

	// post processing buffer
	source = new fwUniform("screenTexture", (glTexture *)nullptr);

	fwUniform *pixelsize = new fwUniform("pixelsize", &m_pixelsize);

	postProcessing = new fwPostProcessing(post_processing + "/vertex.glsl", post_processing + "/fragment.glsl", source, defines);
	postProcessing->addUniform(pixelsize);
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
	m_width = _width;
	m_height = _height;

	glm::vec2 cm = m_renderer->size();
	m_pixelsize.x = 1.0f / cm.x;
	m_pixelsize.y = 1.0f / cm.y;

	SCR_WIDTH = m_width;
	SCR_HEIGHT = m_height;
	resize(m_width, m_height);
	glViewport(0, 0, m_width, m_height);
}

void fwApp::mouseButton(int button, int action)
{
	if (m_control)
		m_control->mouseButton(button, action);
}

void fwApp::mouseMove(double xpos, double ypos)
{
	double x = xpos / m_width;
	double y = ypos / m_height;

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

	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point current_time;
	std::chrono::steady_clock::time_point now_time;
	std::chrono::steady_clock::duration elapsed_time;
	std::chrono::steady_clock::duration time_budget_time = 33ms;
	std::chrono::steady_clock::duration wait_time;
	std::chrono::steady_clock::time_point next_time;

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
		current_time = std::chrono::steady_clock::now();

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

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------

#ifdef _DEBUG
		// Start the Dear ImGui frame
		// Update and Render additional Platform Windows
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		
		if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {

			ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
			//ImGui::DockBuilderGetNode(dockspace_id)->LocalFlags &= ~ImGuiDockNodeFlags_CentralNode;
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

			createDocks(dockspace_id);

			ImGui::DockBuilderFinish(dockspace_id);
		}
		dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
				if (ImGui::MenuItem("New", "Ctrl+N")) {

				}

				if (ImGui::MenuItem("Open...", "Ctrl+O")) {
				}

				if (ImGui::MenuItem("Save", "Ctrl+S")) {

				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {

				}

				if (ImGui::MenuItem("Exit")) {
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();	// DockSpace Demo

		// render your GUI
		if (m_debugger)
			m_debugger->render();
		
		ImGui::Begin("3D View");
		float view3d_height = ImGui::GetWindowSize().x;
		float view3d_width = ImGui::GetWindowSize().y;
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddImage(
			(void*)color->getID(),
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowSize().x - 15,
				ImGui::GetCursorScreenPos().y + ImGui::GetWindowSize().y - 35),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		// when the mouse is in the window, pass events to the App
		bool isHovered = ImGui::IsItemHovered();
		if (isHovered) {
			ImVec2 mousePositionAbsolute = ImGui::GetMousePos();
			ImVec2 screenPositionAbsolute = ImGui::GetItemRectMin();
			ImVec2 mousePositionRelative = ImVec2(mousePositionAbsolute.x - screenPositionAbsolute.x, mousePositionAbsolute.y - screenPositionAbsolute.y);
		}
		ImGui::End();
		
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
#else
		// 3rd pass : post-processing and render on screen
		//glCullFace(GL_BACK);
		postProcessing->draw(color);
#endif
		// render the game
		glfwSwapBuffers(window);

		if (caped_fps) {
			now = GetTickCount64();
			now_time = std::chrono::steady_clock::now();

			elapsed = now - current;
			elapsed_time = now_time - current_time;

			if (elapsed_time < time_budget_time) {
			// if (elapsed < time_budget) {
				wait = time_budget - elapsed;
				wait_time = time_budget_time - elapsed_time;

				std::chrono::steady_clock::time_point n;
				std::chrono::steady_clock::duration w(0);

				//Sleep(wait_time.count() / 1000000);
				//std::this_thread::sleep_for(wait_time);

				do { 
					n = std::chrono::steady_clock::now();
					w = n - now_time;
				} while (w < wait_time );


				//Sleep(wait);
				last_frame_time = time_budget;
				time_budget = 33;
				time_budget_time = 33ms;
				next = current + time_budget;
				next_time = current_time + time_budget_time;
				// s += "short " + std::to_string(elapsed) + " " + std::to_string(wait) + " " + std::to_string(current)  +">>"+ std::to_string(now) +">>"+ std::to_string(next) + "\n";
			}
			else {
				time_budget = 33 * ((elapsed / 33) + 1);
				time_budget_time = 33 * ((elapsed_time / 33) + 1ms);
				next = current + time_budget;
				next_time = current_time + time_budget_time;
				time_budget = next - now;
				time_budget_time = next_time - now_time;
				last_frame_time = elapsed;
				//s += "long " + std::to_string(elapsed) + " " + std::to_string(time_budget) + " " + std::to_string(current) + ">>" + std::to_string(now) + ">>" + std::to_string(next) + "\n";
			}
		}
		else {
			now = GetTickCount64();
			now_time = std::chrono::steady_clock::now();
			last_frame_time = now - current;
		}
	}

#ifdef _DEBUG
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImNodes::DestroyContext();
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
