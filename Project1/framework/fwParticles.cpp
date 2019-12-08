#include "fwParticles.h"

fwParticles::fwParticles(int nb, const std::string& sprite, float radius) : 
	fwSprites(nb)
{
	classID |= PARTICLES;

	m_positions = new glm::vec3[nb];
	m_velocities = new glm::vec3[nb];
	m_timer = new double[nb];
	m_active = new bool[nb];
	m_lifespan = new int[nb];

	for (auto i = 0; i < nb; i++) {
		m_active[i] = false;
	}

	m_image = new fwTexture(sprite);
	set(m_positions, m_image, radius);
}

void fwParticles::update_particle(int &spwanable, int i, double timer)
{
	glm::vec3* position = m_positions + i;
	glm::vec3* velocity = m_velocities + i;
	double *time = m_timer + i;
	bool* active = m_active + i;

	// spawn a new particle if it is empty
	// but only spawn 10 particles per frames
	if (! *active) {
		if (spwanable > 0) {
			velocity->x = (rand() % 10000 - 5000) / 10000.0;
			velocity->y = (rand() % 10000) / 10000.0;
			velocity->z = (rand() % 10000 - 5000) / 10000.0;

			*velocity = glm::normalize(*velocity);

			*velocity *= (rand() % 10);
			*time = 0;
			*active = true;
			spwanable--;
		}
		else {
			return;
		}
	}

	position->x = velocity->x * *time;
	position->y = -0.7 * 9.81 * (*time * *time) + velocity->y * *time;
	position->z = velocity->z * *time;

	*time += timer;

	// despawn particle
	if (position->y + m_Position.y < 0) {
		*active = false;
		spwanable++;
	}
}

void fwParticles::update(double delta)
{
	if (!m_run) {
		// engine stoped
		return;
	}

	double timer = (delta / 5000);	// current time

	int spawnable = 1;

	for (auto i = 0; i < m_size; i++) {

		update_particle(spawnable, i, timer);
	}

	updateVertices();
}

fwParticles::~fwParticles()
{
	delete[] m_positions;
	delete[] m_velocities;
	delete[] m_timer;
	delete[] m_active;
	delete[] m_lifespan;

	delete m_image;
}