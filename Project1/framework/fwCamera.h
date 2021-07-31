#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <queue>

#include "../glEngine/glUniformBuffer.h"
#include "../glEngine/glProgram.h"

#include "fwObject3D.h"
#include "fwFrustum.h"
#include "math/fwSphere.h"

class fwCamera: public fwObject3D
{
protected:
	glm::vec3 target = glm::vec3(0);
	glm::vec3 m_direction = glm::vec3(0);
	glm::vec3 up = glm::vec3(0);
	glm::vec3 right = glm::vec3(0);

	// used to push/pop camera position
	std::queue<glm::vec3> m_positions;
	std::queue<glm::vec3> m_targets;

	glm::mat4 view = glm::mat4(1);
	glm::mat4 m_projection = glm::mat4(1);
	glm::mat4 m_matrix = glm::mat4(1);	// transformation matrix projection * view
	glm::mat4 m_projScreenMatrix = glm::mat4(1);	// projection * inverse(matrix)

	glUniformBuffer *ubo = nullptr;

	fwFrustum m_frustum;

	glm::vec3 unproject(glm::vec3& windowCoordinate);	// unproject the point on the viewport
	virtual void update(void);

public:
	fwCamera();
	fwCamera(int height, int width);
	void set_uniforms(glProgram* program);
	void set_ratio(int width, int height);
	void translate(const glm::vec3 &translation);
	void translate(float x, float y, float z);

	void lookAt(glm::vec3 &target);
	void lookAt(float x, float y, float z);
	glm::vec3 lookAt(void) { return target; };

	void push(void);						// save the camera position
	void pop(void);							// restore the camera position

	glm::mat4 GetViewMatrix(void);
	glm::mat4 GetProjectionMatrix(void);
	glm::mat4 &GetMatrix(void);
	const glm::vec3& direction(void) { return m_direction; };

	bool is_inFrustum(fwMesh *mesh);
	bool is_inFrustum(fwSphere& boundingSphere);

	void set_uniformBuffer(void);
	void bind_uniformBuffer(glProgram *program);

	void rayFromMouse(float x, float y, 
		glm::vec3& ray_ori, glm::vec3& ray_dir);		// return a ray starting from screen coordinate [x,y]
	~fwCamera();
};