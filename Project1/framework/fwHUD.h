#pragma once

#include <string>
#include <list>
#include <map>

#include "fwMaterial.h"
#include "fwFlatPanel.h"

class fwHUDelement;

class fwHUD
{
protected:
	std::string m_name;
	bool m_visible;
	std::list <fwHUDelement*> m_elements;
	fwMaterial* m_material = nullptr;
	fwFlatPanel* m_hudPanel = nullptr;

public:
	fwHUD(const std::string& name, std::map<ShaderType, std::string> *shaders = nullptr, bool visible=true);
	void add(fwHUDelement* element);
	void addUniform(fwUniform* uniform);
	fwMaterial* cloneMaterial(void);
	virtual void draw(void);

	inline bool visible(void) { return m_visible; };
	inline void visible(bool b) { m_visible = b; };

	~fwHUD();
};