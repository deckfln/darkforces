#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glEngine/glProgram.h"
#include "include/stb_image.h"
#include "Material.h"
#include "Camera.h"
#include "Mesh.h"
#include "BoxGeometry.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"

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

	/*
	 * Lights
	 */
	AmbientLight ambient(1.0f, 1.0f, 1.0f, 0.3f);
	DirectionalLight directional(glm::vec3 (10.0f, 0.0f, 10.0f), glm::vec4(1.0f, 1.0f, 1.0f, 0.7f));

	/*
	 * Shader
	 */
	glProgram shaderProgram(vertexShader, fragmentShader);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float _vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};
	GLuint _indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};
	GLfloat _colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f
	};
	GLfloat _texCoords[] = {
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f
	};

	BoxGeometry geometry;
	/*
	geometry.addVertices("aPos", GL_ARRAY_BUFFER, _vertices, 3, sizeof(_vertices), ARRAY_SIZE_OF_ELEMENT(_vertices));
	geometry.addIndex("_index", GL_ELEMENT_ARRAY_BUFFER, _indices, 1, sizeof(_indices), ARRAY_SIZE_OF_ELEMENT(_indices));
	geometry.addAttribute("aTexCoord", GL_ARRAY_BUFFER, _texCoords, 2, sizeof(_texCoords), ARRAY_SIZE_OF_ELEMENT(_texCoords));
	*/

	Material material;
	material.addTexture("ourTexture", "images/crate.png");
	material.addTexture("texture1", "images/brick_diffuse.jpg");
	//material.addAttribute("aColor", GL_ARRAY_BUFFER, _colors, ARRAY_NB_ELEMENTS(_colors), ARRAY_SIZE_OF_ELEMENT(_colors));

	Mesh *cubes[10];
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	for (int i = 0; i < 10; i++) {
		cubes[i] = new Mesh(shaderProgram, geometry, material);
		cubes[i]->translate(cubePositions[i]);
	}
	Mesh cubes(shaderProgram, geometry, material);

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
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

		// draw our first triangle
		shaderProgram.run();

		// set camera
		float radius = 30.0f;
		float x = sin(glfwGetTime()) * radius;
		float z = cos(glfwGetTime()) * radius;
		camera.translate(x, 0, z);

		camera.set_uniforms(shaderProgram);

		// set ambient ligth
		ambient.set_uniform(shaderProgram);
		directional.set_uniform(shaderProgram);

		/*
		Mesh *cube;
		glm::vec3 up(0.5f, 1.0f, 0.0f);
		for (int i = 0; i < 10; i++) {
			cube = cubes[i];
			cube->rotate(i * glfwGetTime() / 100 * glm::radians(50.0f), up);
			cube->draw(shaderProgram);
		}
		*/
		cube.draw(shaderProgram);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	for (int i = 0; i < 10; i++) {
		delete cubes[i];
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}
