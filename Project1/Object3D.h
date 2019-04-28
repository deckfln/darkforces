#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Object3D
{
	glm::mat4 model;

protected:
	glm::vec3 position;

public:
	Object3D();
	void rotate(float angle, glm::vec3 &axis);
	void translate(glm::vec3 &vector);
	~Object3D();
};