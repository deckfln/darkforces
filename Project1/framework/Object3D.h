#pragma once
#include <iostream>
#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Reference.h"

class Object3D : public Reference
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

	std::list <Object3D *> children;

public:
	Object3D();

	bool is_class(int classID);
	Object3D &set_name(std::string _name);

	Object3D &rotate(glm::vec3 &rotation);
	Object3D &translate(glm::vec3 &vector);
	Object3D &translate(float x, float y, float z);

	Object3D &set_scale(glm::vec3 &_scale);
	Object3D &set_scale(float _scale);
	glm::vec3 get_scale(void);

	glm::vec3 &get_position(void);
	std::list <Object3D *> &get_children(void);

	void updateWorldMatrix(Object3D *parent, bool force = false);

	Object3D &addChild(Object3D *);

	~Object3D();
};