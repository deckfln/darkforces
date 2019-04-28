#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glEngine/glProgram.h"
#include "Object3D.h"

class AmbientLight: public Object3D
{
protected:
	glm::vec4 color;

public:
	AmbientLight();
	AmbientLight(glm::vec4 &_color);
	AmbientLight(float r, float g, float b, float strenght);
	void set_uniform(glProgram &program);
	~AmbientLight();
};