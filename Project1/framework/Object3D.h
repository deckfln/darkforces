#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Object3D
{
protected:
	glm::mat4 model;
	glm::vec3 position;
	glm::vec3 scale;
	Object3D &modelMatrix(void);

public:
	Object3D();
	Object3D &rotate(float angle, glm::vec3 &axis);
	Object3D &translate(glm::vec3 &vector);
	Object3D &translate(float x, float y, float z);
	Object3D &set_scale(glm::vec3 &_scale);
	glm::vec3 &get_position(void);
	~Object3D();
};