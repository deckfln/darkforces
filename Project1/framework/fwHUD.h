#pragma once

#include <string>
#include <list>
#include <map>

#include "fwMaterial.h"

class fwHUDelement;

class fwHUD
{
	std::string m_name;
	bool m_visible = true;
	std::list <fwHUDelement*> m_elements;
	
public:
	fwHUD(const std::string& name, std::map<ShaderType, std::string> *shaders = nullptr);
	void add(fwHUDelement* element);
	void addUniform(fwUniform* uniform);
	fwMaterial* cloneMaterial(void);
	void draw(void);

	inline bool visible(void) { return m_visible; };
	inline void visible(bool b) { m_visible = b; };

	~fwHUD();
};