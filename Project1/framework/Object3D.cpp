#include "Object3D.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>


Object3D::Object3D():
	position(0),
	scale(1),
	model(1)
{
}

Object3D &Object3D::set_name(std::string _name)
{
	name = _name;

	return *this;
}

Object3D &Object3D::rotate(glm::vec3 &_rotation)
{
	rotation = _rotation;
	return *this;
}

Object3D &Object3D::translate(glm::vec3 &vector)
{
	position = vector;
	return *this;
}

Object3D &Object3D::translate(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	return *this;
}

Object3D &Object3D::set_scale(glm::vec3 &_scale)
{
	scale = _scale;
	return *this;
}

glm::vec3 Object3D::get_scale(void)
{
	return scale;
}

Object3D &Object3D::set_scale(float _scale)
{
	scale *= _scale;
	return *this;
}

Object3D &Object3D::modelMatrix(void)
{
	glm::mat4 rotationMatrix = glm::rotate(rotation.x, glm::vec3(1, 0, 0));

	glm::mat4 scaleMatrix = glm::scale(scale);
	glm::mat4 translateMatrix = glm::translate(position);
	// model = glm::rotate(model, rotation);
	model = translateMatrix * scaleMatrix * rotationMatrix;

	return *this;
}

glm::vec3 &Object3D::get_position(void)
{
	return position;
}

Object3D::~Object3D()
{
}
