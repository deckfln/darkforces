#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glEngine/glProgram.h"
#include "Object3D.h"

class AmbientLight: public Object3D
{
protected:
	glm::vec3 ambient;

public:
	AmbientLight();
	AmbientLight(glm::vec3 &_color);
	AmbientLight(float r, float g, float b);
	virtual void set_uniform(glProgram &program);
	~AmbientLight();
};