#include "fwObject3D.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

#include "../alEngine/alSource.h"
#include "../alEngine/alSound.h"

fwObject3D::fwObject3D()
{
}

fwObject3D::fwObject3D(const glm::vec3& position):
	m_position(position)
{
}

fwObject3D &fwObject3D::set_name(const std::string& _name)
{
	m_name = _name;

	return *this;
}

bool fwObject3D::is_class(int _classID)
{
	int a= (classID & _classID) != 0;
	return a;
}

fwObject3D &fwObject3D::rotate(const glm::vec3 &_rotation)
{
	m_rotation = _rotation;
	m_quaternion= glm::quat(glm::vec3(_rotation.x, _rotation.y, _rotation.z));
	m_updated = true;
	return *this;
}
fwObject3D& fwObject3D::rotate(glm::vec3 *_rotation)
{
	m_rotation = *_rotation;
	m_quaternion = glm::quat(glm::vec3(_rotation->x, _rotation->y, _rotation->z));
	m_updated = true;
	return *this;
}
fwObject3D& fwObject3D::rotate(glm::quat const* quaternion)
{
	m_quaternion = *quaternion;
	m_updated = true;
	return *this;
}
fwObject3D& fwObject3D::rotate(const glm::quat& quaternion)
{
	m_quaternion = quaternion;
	m_updated = true;
	return *this;
}

/**
 * rotate BY a delta vector
 */
fwObject3D& fwObject3D::rotateBy(const glm::vec3& delta)
{
	m_rotation += delta;
	m_updated = true;
	return *this;
}

fwObject3D& fwObject3D::rotateBy(const glm::vec3* pDelta)
{
	m_rotation += *pDelta;
	m_updated = true;
	return *this;
}

/**
 * Move an object
 */
fwObject3D &fwObject3D::translate(const glm::vec3 &vector)
{
	m_position = vector;
	m_updated = true;
	if (m_source) {
		m_source->position(m_position);
	}
	return *this;
}

fwObject3D &fwObject3D::translate(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
	m_updated = true;
	return *this;
}

fwObject3D& fwObject3D::translate(glm::vec3* pVector)
{
	m_position = *pVector;
	m_updated = true;
	return *this;
}

/**
 * move the position BY a delta vector
 */
fwObject3D& fwObject3D::moveBy(const glm::vec3& delta)
{
	m_position += delta;
	m_updated = true;
	return *this;
}

fwObject3D& fwObject3D::moveBy(const glm::vec3* pDelta)
{
	m_position += *pDelta;
	m_updated = true;
	return *this;
}

/**
 * change the scale of the object
 */
fwObject3D &fwObject3D::set_scale(const glm::vec3 &_scale)
{
	m_scale = _scale;
	m_updated = true;
	return *this;
}

const glm::vec3& fwObject3D::get_scale(void)
{
	return m_scale;
}

fwObject3D &fwObject3D::set_scale(float _scale)
{
	m_updated = true;
	m_scale *= _scale;
	return *this;
}

/**
 * Force a worldMatrix
 */
void fwObject3D::worldMatrix(const glm::mat4& worldMatrix)
{
	m_worldMatrix = worldMatrix;
	m_inverseWorldMatrix = glm::inverse(m_worldMatrix);
	m_updated = false;
}
void fwObject3D::worldMatrix(glm::mat4* pWorldMatrix)
{
	m_worldMatrix = *pWorldMatrix;
	m_inverseWorldMatrix = glm::inverse(m_worldMatrix);
	m_updated = false;
}

fwObject3D &fwObject3D::addChild(fwObject3D *mesh)
{
	m_children.push_front(mesh);
	mesh->m_parent = this;

	return *this;
}

/**
 * remove an object from the scene
 */
void fwObject3D::removeChild(fwObject3D* obj)
{
	m_children.remove(obj);
	obj->m_parent = nullptr;
}

/**
 * distance between the 2 objects
 */
float fwObject3D::distanceTo(fwObject3D* other)
{
	return glm::distance(m_position, other->m_position);
}

/**
 * distance from the entity position to the point
 */
float fwObject3D::distanceTo(const glm::vec3& p)
{
	return glm::distance(m_position, p);
}

/**
 * based on modified attributes, recompute the worldMatrix
 */
void fwObject3D::updateWorldMatrix(fwObject3D *parent, bool force)
{
	//FIXME : checking the updated flag is bad => will not detect the shadowCamera projection
	//if (updated) {
		glm::mat4 rotationMatrix = glm::toMat4(m_quaternion);
		glm::mat4 scaleMatrix = glm::scale(m_scale);
		glm::mat4 translateMatrix = glm::translate(m_position);
		m_modelMatrix = translateMatrix * scaleMatrix * rotationMatrix;
		//updated = false;
		//force = true;
	//}

	if (m_updated || force) {
		if (parent) {
			m_worldMatrix = parent->m_worldMatrix * m_modelMatrix;
			m_inverseWorldMatrix = glm::inverse(m_worldMatrix);
		}
		else {
			m_worldMatrix = m_modelMatrix;
			m_inverseWorldMatrix = glm::inverse(m_worldMatrix);
		}

		force = true;
		m_updated = false;
	}

	for (auto child : m_children) {
		child->updateWorldMatrix(this, force);
	}
}

const glm::vec3 &fwObject3D::get_position(void)
{
	return m_position;
}

const std::list <fwObject3D *> &fwObject3D::get_children(void)
{
	return m_children;
}

/**
 * recursively test if we can find an object
 */
bool fwObject3D::hasChild(fwObject3D* search)
{
	// check first level
	for (auto child : m_children) {
		if (child == search) {
			return true;
		}
	}

	// didn't find so check recursively
	for (auto child : m_children) {
		if (child->hasChild(search)) {
			return true;
		}
	}

	return false;	// sorry, not here
}

/**
 * square root distance between 2 objects
 */
float fwObject3D::sqDistanceTo(fwObject3D *to)
{
	debug = glm::distance2(m_position, to->m_position);

	return debug;
}

/**
 * Play a sound and initialize a source or  Test if the source is still playing that sound

 */
bool fwObject3D::play(alSound* sound)
{
	if (m_source == nullptr) {
		// create a sound source the first time
		m_source = new alSource(m_position);
	}

	return m_source->play(sound);
}

/**
 * Stop playing a sound (if any)
 */
void fwObject3D::stop(alSound *sound)
{
	if (m_source) {
		m_source->stop(sound);
	}
}

fwObject3D::~fwObject3D()
{
	if (m_source) {
		delete m_source;
	}
}
