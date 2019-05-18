#include "fwObject3D.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>


fwObject3D::fwObject3D():
	position(0),
	scale(1),
	modelMatrix(1)
{
}

fwObject3D &fwObject3D::set_name(std::string _name)
{
	name = _name;

	return *this;
}

bool fwObject3D::is_class(int _classID)
{
	return classID & _classID;
}

fwObject3D &fwObject3D::rotate(glm::vec3 &_rotation)
{
	rotation = _rotation;
	updated = true;
	return *this;
}

fwObject3D &fwObject3D::translate(glm::vec3 &vector)
{
	position = vector;
	updated = true;
	return *this;
}

fwObject3D &fwObject3D::translate(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	updated = true;
	return *this;
}

fwObject3D &fwObject3D::set_scale(glm::vec3 &_scale)
{
	scale = _scale;
	updated = true;
	return *this;
}

glm::vec3 fwObject3D::get_scale(void)
{
	return scale;
}

fwObject3D &fwObject3D::set_scale(float _scale)
{
	updated = true;
	scale *= _scale;
	return *this;
}

fwObject3D &fwObject3D::addChild(fwObject3D *mesh)
{
	children.push_front(mesh);
	return *this;
}

void fwObject3D::updateWorldMatrix(fwObject3D *parent, bool force)
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

glm::vec3 &fwObject3D::get_position(void)
{
	return position;
}

std::list <fwObject3D *> &fwObject3D::get_children(void)
{
	return children;
}

fwObject3D::~fwObject3D()
{
}
