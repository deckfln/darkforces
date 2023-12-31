#include "fwObject3D.h"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <imgui.h>

#include "../alEngine/alSource.h"
#include "../alEngine/alSound.h"

static uint32_t g_ids = 0;
static const char* g_className = "fwObject3D";

fwObject3D::fwObject3D() :
	m_id(g_ids++),
	m_className(g_className),
	m_classID(Framework::ClassID::Object3D)
{
}

fwObject3D::fwObject3D(const glm::vec3& position):
	m_position(position),
	m_id(g_ids++),
	m_className(g_className),
	m_classID(Framework::ClassID::Object3D)
{
}

/**
 * push the current transformations
 */
void fwObject3D::pushTransformations(void)
{
	m_saveModelMatrix = m_modelMatrix;
	m_saveWorldMatrix = m_worldMatrix;
	m_saveInverseWorldMatrix = m_inverseWorldMatrix;
	m_savePosition = m_position;
	m_saveScale = m_scale;
	m_saverotation = m_rotation;
	m_saveQuaternion = m_quaternion;
}

/**
 * pop the transformations
 */
void fwObject3D::popTransformations(void)
{
	m_modelMatrix = m_saveModelMatrix;
	m_worldMatrix = m_saveWorldMatrix;
	m_inverseWorldMatrix = m_saveInverseWorldMatrix;
	m_position = m_savePosition;
	m_scale = m_saveScale;
	m_rotation = m_saverotation;
	m_quaternion = m_saveQuaternion;

	m_updated = false;
}

/**
 * set the object name
 */
fwObject3D &fwObject3D::set_name(const std::string& _name)
{
	m_name = _name;

	return *this;
}

bool fwObject3D::is_class(uint32_t classID)
{
	return classID == m_classID;
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
	rotate(m_rotation);
	m_updated = true;
	return *this;
}

fwObject3D& fwObject3D::rotateBy(const glm::vec3* pDelta)
{
	m_rotation += *pDelta;
	rotate(m_rotation);
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
 *
 */
fwObject3D& fwObject3D::transform(const fwTransforms& transforms)
{
	m_position = transforms.m_position;
	m_scale = transforms.m_scale;
	m_quaternion = transforms.m_quaternion;
	m_updated = true;

	return *this;
}

fwObject3D& fwObject3D::transform(fwTransforms const* pTransforms)
{
	m_position = pTransforms->m_position;
	m_scale = pTransforms->m_scale;
	m_quaternion = pTransforms->m_quaternion;
	m_updated = true;

	return *this;
}

/**
 * Force a worldMatrix
 */
void fwObject3D::worldMatrix(const glm::mat4& worldMatrix, const glm::mat4& inverseWorldMatrix)
{
	m_worldMatrix = worldMatrix;
	m_inverseWorldMatrix = inverseWorldMatrix;
	m_updated = false;
}
void fwObject3D::worldMatrix(const glm::mat4& worldMatrix)
{
	m_worldMatrix = worldMatrix;
	m_inverseWorldMatrix = glm::inverse(worldMatrix);
	m_updated = false;
}
void fwObject3D::worldMatrix(glm::mat4* pWorldMatrix, glm::mat4* pInverseWorldMatrix)
{
	m_worldMatrix = *pWorldMatrix;
	m_inverseWorldMatrix = *pInverseWorldMatrix;
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

bool fwObject3D::visible(void)
{
	if (m_parent) {
		return m_visible && m_parent->visible();
	}
	return m_visible;
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
		m_source = new alSource();
	}

	return m_source->play(sound, m_position);
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

/**
 * Record the state of the object
 */
uint32_t fwObject3D::recordState(flightRecorder::Object3D* record)
{
	record->position = m_position;
	record->scale = m_scale;
	record->rotation = m_rotation;
	record->quaternion = m_quaternion;

	return sizeof(flightRecorder::Object3D);
}

/**
 * reload the state of the object from the flight recorder
 */
void fwObject3D::loadState(flightRecorder::Object3D* record)
{
	m_position = record->position;
	m_scale = record->scale;
	m_rotation = record->rotation;
	m_quaternion = record->quaternion;

	m_updated = true;	// force to rebuild the transforms matrixes
}

/**
 * return user friendly class name
 */
const char *fwObject3D::className(void)
{
	return m_className;
}

static char tmp[64];

/**
 * Display the object alone in the debugger
 */
void fwObject3D::debugGUI(void)
{
	if (m_name != "") {
		_snprintf_s(tmp, sizeof(tmp), _TRUNCATE, "%s (%d) \"%s\"", m_className, m_id, m_name.c_str());
	}
	else {
		_snprintf_s(tmp, sizeof(tmp), _TRUNCATE, "%s (%d)", m_className, m_id);
	}

	if (ImGui::CollapsingHeader(tmp)) {
		debugGUIChildClass();
	}
}

/**
 * Add dedicated component debug the entity
 */
void fwObject3D::debugGUIChildClass(void)
{
	if (ImGui::TreeNode("fwObject3D")) {
		if (m_name != "") {
			ImGui::Text(m_name.c_str());
		}
		ImGui::Text("Position %.2f %.2f %.2f", m_position.x, m_position.y, m_position.z);
		ImGui::Text("Rotation %.2f %.2f %.2f", m_rotation.x, m_rotation.y, m_rotation.z);
		ImGui::Text("Scale    %.2f %.2f %.2f", m_scale.x, m_scale.y, m_scale.z);
		ImGui::Checkbox("Visible", &m_visible);
		ImGui::TreePop();
	}
}

/**
 *
 */
void fwObject3D::debugGUItree(std::map<fwObject3D*, bool>& inspector)
{
	if (m_name != "") {
		_snprintf_s(tmp, sizeof(tmp), _TRUNCATE, "%s (%d) \"%s\"", m_className, m_id, m_name.c_str());
	}
	else {
		_snprintf_s(tmp, sizeof(tmp), _TRUNCATE, "%s (%d)", m_className, m_id);
	}

	if (m_children.size() > 0) {
		if (ImGui::TreeNode(tmp)) {
			ImGui::Checkbox(tmp, &inspector[this]);
			for (auto child : m_children) {
				child->debugGUItree(inspector);
			}
			ImGui::TreePop();
		}
	}
	else {
		ImGui::Checkbox(tmp, &inspector[this]);
	}
}

fwObject3D::~fwObject3D()
{
	if (m_source) {
		delete m_source;
	}
}
