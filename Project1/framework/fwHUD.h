#pragma once

#include <list>

class fwHUDelement;

class fwHUD
{
	std::list <fwHUDelement*> m_elements;
	
public:
	fwHUD();
	void add(fwHUDelement* element);
	void draw(void);
	~fwHUD();
};