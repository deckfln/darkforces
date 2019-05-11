#include "Object3D.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>


Object3D::Object3D():
	position(0),
	scale(1),
	modelMatrix(1)
{
}

Object3D &Object3D::set_name(std::string _name)
{
	name = _name;

	return *this;
}

bool Object3D::is_class(int _classID)
{
	return classID & _classID;
}

Object3D &Object3D::rotate(glm::vec3 &_rotation)
{
	rotation = _rotation;
	updated = true;
	return *this;
}

Object3D &Object3D::translate(glm::vec3 &vector)
{
	position = vector;
	updated = true;
	return *this;
}

Object3D &Object3D::translate(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	updated = true;
	return *this;
}

Object3D &Object3D::set_scale(glm::vec3 &_scale)
{
	scale = _scale;
	updated = true;
	return *this;
}

glm::vec3 Object3D::get_scale(void)
{
	return scale;
}

Object3D &Object3D::set_scale(float _scale)
{
	updated = true;
	scale *= _scale;
	return *this;
}

Object3D &Object3D::addChild(Object3D *mesh)
{
	children.push_front(mesh);
	return *this;
}

void Object3D::updateWorldMatrix(Object3D *parent, bool force)
{
	if (updated) {
		glm::mat4 rotationMatrix = glm::rotate(rotation.x, glm::vec3(1, 0, 0));

		glm::mat4 scaleMatrix = glm::scale(scale);
		glm::mat4 translateMatrix = glm::translate(position);
		// model = glm::rotate(model, rotation);
		modelMatrix = translateMatrix * scaleMatrix * rotationMatrix;
	}

	if (updated or force) {
		if (parent) {
			worldMatrix = parent->worldMatrix * modelMatrix;
		}
		else {
			worldMatrix = modelMatrix;
		}

		force = true;
	}

	for (auto child : children) {
		child->updateWorldMatrix(this, force);
	}
}

glm::vec3 &Object3D::get_position(void)
{
	return position;
}

std::list <Object3D *> &Object3D::get_children(void)
{
	return children;
}

Object3D::~Object3D()
{
}
