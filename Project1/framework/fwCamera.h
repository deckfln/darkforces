#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../glEngine/glUniformBuffer.h"
#include "../glEngine/glProgram.h"

#include "fwObject3D.h"
#include "fwFrustum.h"

class fwCamera: public fwObject3D
{
protected:
	glm::vec3 target;
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 right;

	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 m_matrix;	// transformation matrix projection * view
	glm::mat4 m_projScreenMatrix;	// projection * inverse(matrix)

	glUniformBuffer *ubo = nullptr;

	fwFrustum m_frustum;

	void update(void);

public:
	fwCamera();
	fwCamera(int height, int width);
	void set_ratio(int width, int height);
	void set_uniforms(glProgram *program);
	void translate(glm::vec3 &translation);
	void translate(float x, float y, float z);
	void lookAt(glm::vec3 &target);
	void lookAt(float x, float y, float z);

	glm::mat4 GetViewMatrix(void);
	glm::mat4 GetProjectionMatrix(void);
	glm::mat4 &GetMatrix(void);

	bool is_inFrustum(fwMesh *mesh);

	void set_uniformBuffer(void);
	void bind_uniformBuffer(glProgram *program);

	~fwCamera();
};