#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glEngine/glProgram.h"
#include "include/stb_image.h"
#include "framework/DiffuseMaterial.h"
#include "framework/MaterialBasic.h"
#include "framework/Camera.h"
#include "framework/Mesh.h"
#include "framework/BoxGeometry.h"
#include "framework/DirectionLight.h"
#include "framework/PointLight.h"
#include "framework/SpotLight.h"
#include "framework/Scene.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

const std::string vertexShader = "shaders/vertex.glsl";
const std::string fragmentShader = "shaders/fragment.glsl";

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
	Camera camera(SCR_WIDTH, SCR_HEIGHT);
	camera.lookAt(0, 0, 0);
	camera.translate(3, 3, 3);

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
		glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		1.0,
		0.09,
		0.032
		);
	
	/*
	SpotLight light(
		glm::vec3(0),
		glm::vec3(0,0,-1),

		glm::vec3(0.2f, 0.2f, 0.2f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		1.0,
		0.09,
		0.032,
		glm::cos(glm::radians(12.5f)),
		glm::cos(glm::radians(17.5f))
	);
	*/
	BoxGeometry geometry;

	Texture *t1 = new Texture("images/container2.png");
	Texture *t2 = new Texture("images/container2_specular.png");
	DiffuseMaterial material(t1, t2, 64);

	Mesh cube(geometry, &material);
	Mesh cube1(geometry, &material);
	cube1.translate(0.5, 0, -3);

	glm::vec4 white(1.0);
	MaterialBasic basic(white);
	Mesh fLight(geometry, &basic);

	glm::vec3 half(0.1);
	fLight.set_scale(half);

	Scene scene;
	scene.addCamera(&camera).
		addLight(&light).
		addMesh(&cube).
		addMesh(&cube1).
		addMesh(&fLight);

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

		// render
		// ------
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float radius = 2;
		lightPos.x = sin(glfwGetTime() / 2) * radius;
		lightPos.z = cos(glfwGetTime() / 2) * radius;
		light.translate(lightPos);

		fLight.translate(lightPos);

		scene.draw();

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
