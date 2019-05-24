#include "Camera.h"



Camera::Camera(int width, int height):
	up(0.0f, 1.0f, 0.0f)
{
	view = glm::mat4(1.0f);

	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
}

void Camera::set_ratio(int width, int height)
{
	projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
}

void Camera::translate(glm::vec3 &translation)
{
	position = translation;

	direction = glm::normalize(position - target);
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(_up, direction));
	up = glm::cross(direction, right);

	view = glm::lookAt(position, target, up);
}

void Camera::translate(const float x, const float y, const float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	direction = glm::normalize(position - target);
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(_up, direction));
	up = glm::cross(direction, right);

	view = glm::lookAt(position, target, up);
}

void Camera::lookAt(glm::vec3 &_target)
{
	target = _target;
	direction = glm::normalize(position - target);
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(_up, direction));
	up = glm::cross(direction, right);

	view = glm::lookAt(position, target, up);
}

void Camera::lookAt(float x, float y, float z)
{
	target.x = x;
	target.y = y;
	target.z = z;

	direction = glm::normalize(position - target);
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	right = glm::normalize(glm::cross(_up, direction));
	up = glm::cross(direction, right);

	view = glm::lookAt(position, target, up);
}

glm::mat4 Camera::GetViewMatrix(void)
{
	return glm::lookAt(position, right, up);
}

glm::mat4 Camera::GetProjectionMatrix(void)
{
	return projection;
}

void Camera::set_uniforms(glProgram *program)
{
	program->set_uniform("view", view);
	program->set_uniform("projection", projection);
	program->set_uniform("viewPos", position);
}

void Camera::set_uniformBuffer(void)
{
	if (ubo == nullptr) {
		ubo = new glUniformBuffer(2 * sizeof(glm::mat4) + sizeof(glm::vec3) + 4);
	}

	glm::mat4 camera[2] = { view, projection };

	ubo->bind();
	ubo->map(glm::value_ptr(camera[0]), 0, sizeof(camera));
	ubo->map(glm::value_ptr(position), sizeof(camera), sizeof(position));
	ubo->bind();
}

void Camera::bind_uniformBuffer(glProgram *program)
{
	ubo->bind(program, "Camera");
}

Camera::~Camera()
{
	if (ubo != nullptr) {
		delete ubo;
	}
}