#include "Object3D.h"



Object3D::Object3D()
{
}

void Object3D::rotate(float t, glm::vec3 &axis)
{
	model = glm::rotate(model, t, axis);
}

void Object3D::translate(glm::vec3 &vector)
{
	model = glm::translate(model, vector);
}

Object3D::~Object3D()
{
}
