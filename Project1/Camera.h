#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glEngine/glProgram.h"

class Camera
{
	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 right;

	glm::mat4 view;
	glm::mat4 projection;
public:
	Camera(int height, int width);
	void set_uniforms(glProgram &program);
	void translate(glm::vec3 &translation);
	void translate(float x, float y, float z);
	void lookAt(glm::vec3 &target);
	void lookAt(float x, float y, float z);
	~Camera();
};