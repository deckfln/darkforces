#include "fwCamera.h"

#include <glm/gtx/matrix_decompose.hpp>
#include "../alEngine/alListener.h"

fwCamera::fwCamera()
{

}

fwCamera::fwCamera(int width, int height):
	up(0.0f, 1.0f, 0.0f)
{
	view = glm::mat4(1.0f);

	m_projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 400.0f);
	m_matrix = m_projection * view;

	update();
}

void fwCamera::set_ratio(int width, int height)
{
	m_projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 400.0f);
	m_matrix = m_projection * view;
	//m_projScreenMatrix = m_projection * inverse(m_matrix);
	//m_frustum.setFromMatrix(m_projScreenMatrix);
	update();
}

void fwCamera::translate(const glm::vec3 &translation)
{
	fwObject3D::translate(translation);
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
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);

	m_direction = position() - target;
	if (m_direction != glm::vec3(0)) {
		m_direction = glm::normalize(m_direction);
		right = glm::normalize(glm::cross(_up, m_direction));
		up = glm::cross(m_direction, right);
	}
	else {
		up = _up;
	}

	g_Listener.position(position());
	g_Listener.orientation(m_direction, up);

	// from learn-opengl
	view = glm::lookAt(position(), target, up);

	// from three.js: something to fix down the line
	glm::mat4 THREEjs;
	THREEjs[0][0] = right.x; THREEjs[1][0] = up.x; THREEjs[2][0] = m_direction.x;
	THREEjs[0][1] = right.y; THREEjs[1][1] = up.y; THREEjs[2][1] = m_direction.y;
	THREEjs[0][2] = right.z; THREEjs[1][2] = up.z; THREEjs[2][2] = m_direction.z;

	glm::quat q = glm::quatLookAt(-m_direction, up);
	rotate(q);

	m_matrix = m_projection * view;

	updateWorldMatrix(nullptr, false);
	m_projScreenMatrix = m_projection * glm::inverse(m_worldMatrix);

	m_frustum.setFromMatrix(m_projScreenMatrix);
}

glm::mat4 fwCamera::GetViewMatrix(void)
{
	return view;
}

glm::mat4 fwCamera::GetProjectionMatrix(void)
{
	return m_projection;
}

glm::mat4 &fwCamera::GetMatrix(void)
{
	return m_matrix;
}

/**
 * Save the camera position
 */
void fwCamera::push(void)
{
	m_positions.push(position());
	m_targets.push(target);
}

/**
 * restore the camera position
 */
void fwCamera::pop(void)
{
	translate( m_positions.front() );
	target = m_targets.front();

	m_positions.pop();
	m_targets.pop();

	update();
}

void fwCamera::set_uniforms(glProgram *program)
{
	program->set_uniform("view", view);
	program->set_uniform("projection", m_projection);
	program->set_uniform("viewPos", position());
}

void fwCamera::set_uniformBuffer(void)
{
	if (ubo == nullptr) {
		ubo = new glUniformBuffer(2 * sizeof(glm::mat4) + sizeof(glm::vec3) + 4);
	}

	glm::mat4 camera[2] = { view, m_projection };
	glm::vec3& p = (glm::vec3 &)position();
	ubo->bind();
	ubo->map(glm::value_ptr(camera[0]), 0, sizeof(camera));
	ubo->map(glm::value_ptr(p), sizeof(camera), sizeof(p));
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

/**
 * Test a WORLD positioning boundingSphere against the frustum
 * TODO convert the boundingSphere to a 3D object (Mesh, Object3D) than can be moved around
 */
bool fwCamera::is_inFrustum(fwSphere& boundingSphere)
{
	return m_frustum.intersectsSphere(boundingSphere, boundingSphere);
}

/**
 * return a ray starting from screen coordinate [x,y]
 * https://gdbooks.gitbooks.io/3dcollisions/content/Chapter5/picking.html
 */
void fwCamera::rayFromMouse(float x, float y, glm::vec3& ray_ori, glm::vec3& ray_dir)
{
	glm::vec4 viewport( 0.0f, 0.0f, 1.0f, 1.0f );

	glm::vec3 pnear = glm::unProject(glm::vec3(x, y, 0.0f), view, m_projection, viewport);
	glm::vec3 pfar = glm::unProject(glm::vec3(x, y, 1.0f), view, m_projection, viewport);

	ray_ori = pnear;
	ray_dir = pfar - pnear;
}

fwCamera::~fwCamera()
{
	if (ubo != nullptr) {
		delete ubo;
	}
}