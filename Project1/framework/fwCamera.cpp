#include "fwCamera.h"


fwCamera::fwCamera()
{

}

fwCamera::fwCamera(int width, int height):
	up(0.0f, 1.0f, 0.0f)
{
	view = glm::mat4(1.0f);

	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	m_matrix = projection * view;
}

void fwCamera::set_ratio(int width, int height)
{
	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	m_matrix = projection * view;
}

void fwCamera::translate(glm::vec3 &translation)
{
	m_Position = translation;

	direction = glm::normalize(m_Position - target);
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(_up, direction));
	up = glm::cross(direction, right);

	view = glm::lookAt(m_Position, target, up);
	m_matrix = projection * view;
}

void fwCamera::translate(const float x, const float y, const float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;

	direction = glm::normalize(m_Position - target);
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(_up, direction));
	up = glm::cross(direction, right);

	view = glm::lookAt(m_Position, target, up);
	m_matrix = projection * view;
}

void fwCamera::lookAt(glm::vec3 &_target)
{
	target = _target;
	direction = glm::normalize(m_Position - target);
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(_up, direction));
	up = glm::cross(direction, right);

	view = glm::lookAt(m_Position, target, up);
	m_matrix = projection * view;
}

void fwCamera::lookAt(float x, float y, float z)
{
	target.x = x;
	target.y = y;
	target.z = z;

	direction = glm::normalize(m_Position - target);
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(_up, direction));
	up = glm::cross(direction, right);

	view = glm::lookAt(m_Position, target, up);
	m_matrix = projection * view;
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
	ubo->bind();
}

void fwCamera::bind_uniformBuffer(glProgram *program)
{
	ubo->bind(program, "Camera");
}

fwCamera::~fwCamera()
{
	if (ubo != nullptr) {
		delete ubo;
	}
}