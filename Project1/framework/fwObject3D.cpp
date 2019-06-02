#include "fwObject3D.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

fwObject3D::fwObject3D():
	m_Position(0),
	m_Scale(1),
	m_modelMatrix(1)
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
	m_Rotation = _rotation;
	m_quaternion= glm::quat(glm::vec3(_rotation.x, _rotation.y, _rotation.z));
	updated = true;
	return *this;
}

fwObject3D &fwObject3D::translate(glm::vec3 &vector)
{
	m_Position = vector;
	updated = true;
	return *this;
}

fwObject3D &fwObject3D::translate(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
	updated = true;
	return *this;
}

fwObject3D &fwObject3D::set_scale(glm::vec3 &_scale)
{
	m_Scale = _scale;
	updated = true;
	return *this;
}

glm::vec3 fwObject3D::get_scale(void)
{
	return m_Scale;
}

fwObject3D &fwObject3D::set_scale(float _scale)
{
	updated = true;
	m_Scale *= _scale;
	updated = true;
	return *this;
}

fwObject3D &fwObject3D::addChild(fwObject3D *mesh)
{
	m_children.push_front(mesh);
	return *this;
}

void fwObject3D::updateWorldMatrix(fwObject3D *parent, bool force)
{
	//FIXME : checking the updated flag is bad => will not detect the shadowCamera projection
	//if (updated) {
		//glm::mat4 rotationMatrix = glm::rotate(m_Rotation.x, glm::vec3(1, 0, 0));
		glm::mat4 rotationMatrix = glm::toMat4(m_quaternion);
		glm::mat4 scaleMatrix = glm::scale(m_Scale);
		glm::mat4 translateMatrix = glm::translate(m_Position);
		// model = glm::rotate(model, rotation);
		m_modelMatrix = translateMatrix * scaleMatrix * rotationMatrix;
		//updated = false;
		//force = true;
	//}

	if (updated || force) {
		if (parent) {
			m_worldMatrix = parent->m_worldMatrix * m_modelMatrix;
		}
		else {
			m_worldMatrix = m_modelMatrix;
		}

		force = true;
	}

	for (auto child : m_children) {
		child->updateWorldMatrix(this, force);
	}
}

glm::vec3 &fwObject3D::get_position(void)
{
	return m_Position;
}

std::list <fwObject3D *> &fwObject3D::get_children(void)
{
	return m_children;
}

/*
 */
float fwObject3D::sqDistanceTo(fwObject3D *to)
{
	debug = glm::distance2(m_Position, to->m_Position);

	return debug;
}

fwObject3D::~fwObject3D()
{
}
