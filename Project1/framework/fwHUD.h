#pragma once

#include <list>
#include <map>

#include "fwMaterial.h"

class fwHUDelement;

class fwHUD
{
	std::list <fwHUDelement*> m_elements;
	
public:
	fwHUD(std::map<ShaderType, std::string> *shaders = nullptr);
	void add(fwHUDelement* element);
	void addUniform(fwUniform* uniform);
	void draw(void);
	~fwHUD();
};