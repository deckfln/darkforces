#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Reference.h"

class Object3D : public Reference
{
protected:
	std::string name;

	glm::mat4 model;
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;

	Object3D &modelMatrix(void);

public:
	Object3D();
	Object3D &set_name(std::string _name);

	Object3D &rotate(glm::vec3 &rotation);
	Object3D &translate(glm::vec3 &vector);
	Object3D &translate(float x, float y, float z);

	Object3D &set_scale(glm::vec3 &_scale);
	Object3D &set_scale(float _scale);
	glm::vec3 get_scale(void);

	glm::vec3 &get_position(void);
	~Object3D();
};