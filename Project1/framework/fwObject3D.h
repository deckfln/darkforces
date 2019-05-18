#pragma once
#include <iostream>
#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Reference.h"

class fwObject3D : public Reference
{
protected:
	std::string name;
	int classID = 0;

	glm::mat4 modelMatrix;	// model space matrix
	glm::mat4 worldMatrix;	// world space matrix (including children)

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;

	bool updated = true;	// if the matrix components have been update (or are new)

	std::list <fwObject3D *> children;

public:
	fwObject3D();

	bool is_class(int classID);
	fwObject3D &set_name(std::string _name);

	fwObject3D &rotate(glm::vec3 &rotation);
	fwObject3D &translate(glm::vec3 &vector);
	fwObject3D &translate(float x, float y, float z);

	fwObject3D &set_scale(glm::vec3 &_scale);
	fwObject3D &set_scale(float _scale);
	glm::vec3 get_scale(void);

	glm::vec3 &get_position(void);
	std::list <fwObject3D *> &get_children(void);

	void updateWorldMatrix(fwObject3D *parent, bool force = false);

	fwObject3D &addChild(fwObject3D *);

	~fwObject3D();
};