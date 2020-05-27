#pragma once
#include <iostream>
#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../Reference.h"

class fwObject3D : public Reference
{
	float debug = 0;

protected:
	std::string m_name;
	int classID = 0;

	glm::mat4 m_modelMatrix;						// model space matrix
	glm::mat4 m_worldMatrix = glm::mat4(0);			// world space matrix (including children)
	glm::mat4 m_inverseWorldMatrix = glm::mat4(0);	// world space matrix (including children)


	glm::vec3 m_Position = glm::vec3(0);
	glm::vec3 m_Scale = glm::vec3(0);
	glm::vec3 m_Rotation = glm::vec3(0);
	glm::quat m_quaternion;

	bool updated = true;							// if the matrix components have been update (or are new)

	std::list <fwObject3D *> m_children;

	bool m_castShadow = false;
	bool m_receiveShadow = false;

public:
	fwObject3D();

	bool is_class(int classID);
	fwObject3D &set_name(std::string _name);
	std::string& name(void) { return m_name;	};

	fwObject3D &rotate(glm::vec3 &rotation);
	fwObject3D &translate(glm::vec3 &vector);
	fwObject3D &translate(float x, float y, float z);

	fwObject3D &set_scale(glm::vec3 &_scale);
	fwObject3D &set_scale(float _scale);
	glm::vec3 get_scale(void);

	glm::vec3 &get_position(void);
	//void position(glm::vec3& position) { m_Position = position; };
	std::list <fwObject3D *> &get_children(void);
	bool hasChild(fwObject3D* child);

	void updateWorldMatrix(fwObject3D *parent, bool force = false);

	bool castShadow(void) { return m_castShadow; }
	virtual bool castShadow(bool s) { m_castShadow = s; return s; }

	bool receiveShadow(void) { return m_receiveShadow; }
	bool receiveShadow(bool s) { m_receiveShadow = s; return s; }

	glm::mat4 &worldMatrix(void) { return m_worldMatrix; }
	void worldMatrix(glm::mat4& worldMatrix);

	glm::mat4& inverseWorldMatrix(void) { return m_inverseWorldMatrix; }

	fwObject3D &addChild(fwObject3D *);

	float sqDistanceTo(fwObject3D *to);

	~fwObject3D();
};