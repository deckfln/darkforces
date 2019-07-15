#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../glEngine/glProgram.h"
#include "fwPointLight.h"

class fwSpotLight: public fwPointLight
{
protected:
	glm::vec3 direction;
	float cutoff;
	float outerCutoff;

public:
	fwSpotLight();
	fwSpotLight(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular, float constant, float linear, float quadatric, float _cutoff, float _outerCutoff=0);
	std::string set_uniform(glProgram *program, int index = -1);
	~fwSpotLight();
};