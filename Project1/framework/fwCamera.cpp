#include "fwCamera.h"

#include <glm/gtx/matrix_decompose.hpp>

fwCamera::fwCamera()
{

}

fwCamera::fwCamera(int width, int height):
	up(0.0f, 1.0f, 0.0f)
{
	view = glm::mat4(1.0f);

	projection = glm::perspective(glm::radians(25.0f), (float)width / (float)height, 1.0f, 1000.0f);
	m_matrix = projection * view;

	update();
}

void fwCamera::set_ratio(int width, int height)
{
	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	m_matrix = projection * view;
	m_projScreenMatrix = projection * inverse(m_matrix);

	m_frustum.setFromMatrix(m_projScreenMatrix);
}

void fwCamera::translate(glm::vec3 &translation)
{
	m_Position = translation;
	update();
}

void fwCamera::translate(const float x, const float y, const float z)
{
	fwObject3D::translate(x, y, z);

	update();
}

void fwCamera::lookAt(glm::vec3 &_target)
{
	target = _target;
	update();
}

void fwCamera::lookAt(float x, float y, float z)
{
	target.x = x;
	target.y = y;
	target.z = z;

	update();
}

void fwCamera::update(void)
{
	direction = glm::normalize(m_Position - target);
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(_up, direction));
	up = glm::cross(direction, right);

	// from learn-opengl
	view = glm::lookAt(m_Position, target, up);

	// from three.js: something to fix down the line
	glm::mat4 THREEjs;
	THREEjs[0][0] = right.x; THREEjs[1][0] = up.x; THREEjs[2][0] = direction.x;
	THREEjs[0][1] = right.y; THREEjs[1][1] = up.y; THREEjs[2][1] = direction.y;
	THREEjs[0][2] = right.z; THREEjs[1][2] = up.z; THREEjs[2][2] = direction.z;

	m_quaternion = glm::quatLookAt(-direction, up);

	m_matrix = projection * view;

	updateWorldMatrix(nullptr, false);
	m_projScreenMatrix = projection * glm::inverse(m_worldMatrix);

	m_frustum.setFromMatrix(m_projScreenMatrix);
}

glm::mat4 fwCamera::GetViewMatrix(void)
{
	return view;
}

glm::mat4 fwCamera::GetProjectionMatrix(void)
{
	return projection;
}

glm::mat4 &fwCamera::GetMatrix(void)
{
	return m_matrix;
}

void fwCamera::set_uniforms(glProgram *program)
{
	program->set_uniform("view", view);
	program->set_uniform("projection", projection);
	program->set_uniform("viewPos", m_Position);
}

void fwCamera::set_uniformBuffer(void)
{
	if (ubo == nullptr) {
		ubo = new glUniformBuffer(2 * sizeof(glm::mat4) + sizeof(glm::vec3) + 4);
	}

	glm::mat4 camera[2] = { view, projection };

	ubo->bind();
	ubo->map(glm::value_ptr(camera[0]), 0, sizeof(camera));
	ubo->map(glm::value_ptr(m_Position), sizeof(camera), sizeof(m_Position));
	ubo->unbind();
}

void fwCamera::bind_uniformBuffer(glProgram *program)
{
	ubo->bind(program, "Camera");
}

bool fwCamera::is_inFrustum(fwMesh *mesh)
{
	return m_frustum.intersectsObject(mesh);
}

fwCamera::~fwCamera()
{
	if (ubo != nullptr) {
		delete ubo;
	}
}