#pragma once

#include "fwSprites.h"

#define PARTICLES 8

class fwParticles : public fwSprites
{
protected:
	glm::vec3 *m_positions = nullptr;
	glm::vec3 *m_velocities = nullptr;
	double *m_timer = nullptr;
	bool* m_active = nullptr;
	int* m_lifespan = nullptr;
	bool m_run = false;
	fwTexture* m_image;

public:
	fwParticles(int nb, const std::string &sprite, float radius);
	virtual void update_particle(int& spwanable, int i, double timer);
	void update(double delta);
	void stop(void) { m_run = false; }
	void start(void) { m_run = true; }
	~fwParticles();
};
