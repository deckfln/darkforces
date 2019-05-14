#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

#include "glEngine/glProgram.h"
#include "include/stb_image.h"
#include "framework/DiffuseMaterial.h"
#include "framework/MaterialBasic.h"
#include "framework/Camera.h"
#include "framework/Mesh.h"
#include "framework/BoxGeometry.h"
#include "framework/geometries/PlaneGeometry.h"
#include "framework/DirectionLight.h"
#include "framework/PointLight.h"
#include "framework/SpotLight.h"
#include "framework/Scene.h"
#include "framework/FrameBuffer.h"
#include "framework/fwPostProcessing.h"
#include "framework/fwSkybox.h"
#include "framework/controls/fwOrbitControl.h"

#include "framework/Loader.h"

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

Camera camera(SCR_WIDTH, SCR_HEIGHT);
int Button = 0;

fwOrbitControl control(&camera);

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	camera.set_ratio(width, height);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
		switch (action) {
		case GLFW_PRESS:
			Button = GLFW_MOUSE_BUTTON_RIGHT;
			break;
		case GLFW_RELEASE:
			Button = 0;
			break;
		}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	float x = xpos / SCR_WIDTH;
	float y = ypos / SCR_HEIGHT;

	control.mouseEvent(Button, x, y);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

const std::string vertexShader = "shaders/vertex.glsl";
const std::string fragmentShader = "shaders/fragment.glsl";

int main()
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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/*
	 * Camera
	 */
	camera.lookAt(0, 2, 0);
	camera.translate(5, 5, 5);

	/*
	 * Lights
	 */
	/*
	DirectionLight light(
		glm::vec3(0.0),
		glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	);
	*/

	PointLight light(
		glm::vec3(), 
		glm::vec3(0, 0.2f, 0),
		glm::vec3(0, 0.5f, 0),
		glm::vec3(0, 1.0f, 0),
		1.0,
		0.09,
		0.032
		);

	SpotLight light2(
		glm::vec3(0),
		glm::vec3(0,0,-1),

		glm::vec3(0.2f, 0, 0),
		glm::vec3(0.5f, 0, 0),
		glm::vec3(1.0f, 1, 1),
		1.0,
		0.09,
		0.032,
		glm::cos(glm::radians(12.5f)),
		glm::cos(glm::radians(17.5f))
	);

	Loader loader("models/stormtrooper/stormtrooper.dae");
	std::vector<Mesh *>meshes = loader.get_meshes();

	// lights
	BoxGeometry *geometry = new BoxGeometry();
	glm::vec4 white(1.0);

	MaterialBasic *basic = new MaterialBasic(&white);
	Mesh fLight(geometry, basic);
	Mesh fLight2(geometry, basic);
	
	glm::vec3 half(0.1);
	fLight.set_scale(half).set_name("light");
	fLight2.set_scale(half).set_name("light2");

	light.addChild(&fLight);
	light2.addChild(&fLight2);

	// floor
	Texture *t1 = new Texture("images/container2.png");
	Texture *t2 = new Texture("images/container2_specular.png");
	DiffuseMaterial *material = new DiffuseMaterial(t1, nullptr, 64);

	Mesh plane(new PlaneGeometry(8, 8), material);

	glm::vec3 deg(3.1415 / 2, 0, 0);
	plane.rotate(deg);

	// init the scene
	glm::vec4 yellow(255, 255, 0, 255);
	Scene scene;
	scene.addCamera(&camera).
		addLight(&light).
		addLight(&light2).
		setOutline(&yellow).
		addChild(&plane);

	for (auto mesh : meshes) {
		mesh->outline(true);
		scene.addChild(mesh);
	}

	// FRAME BUFFER
	FrameBuffer frameBuffer(SCR_WIDTH, SCR_HEIGHT);
	glTexture *tex = frameBuffer.get_colorBuffer();
	Uniform source("screenTexture", tex);
	fwPostProcessing postProcessing("shaders/screen_vertex.glsl", "shaders/screen_fragment.glsl", &source);

	// Skybox
	std::string skyboxes[] = { 
		"images/skybox/right.jpg",
		"images/skybox/left.jpg",
		"images/skybox/top.jpg",
		"images/skybox/bottom.jpg",
		"images/skybox/front.jpg",
		"images/skybox/back.jpg" };

	fwSkybox skybox(skyboxes);

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	glm::vec3 lightPos(0,0,0);

	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render 1st pass

		frameBuffer.bind();

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		float radius = 2;
		lightPos.x = sin(glfwGetTime() / 2) * radius;
		lightPos.y = 1;
		lightPos.z = cos(glfwGetTime() / 2) * radius;
		light.translate(lightPos);

		lightPos.x = sin(glfwGetTime() / 4) * radius;
		lightPos.y = 3;
		lightPos.z = cos(glfwGetTime() / 4) * radius;
		light2.translate(lightPos);

		scene.draw();

		skybox.draw(&camera);

		frameBuffer.unbind();

		// render 2nd pass
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		postProcessing.draw();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}
