#include "fwParticles.h"

static float _steel_colors[12][3] = {
	{254, 247, 237},
	{251, 238, 219},
	{255, 236, 178},
	{249, 226, 146},
	{249, 206, 104},
	{239, 137, 65},
	{231, 95, 46},
	{229, 71, 36},
	{202, 56, 33},
	{157, 32, 28},
	{98, 20, 18},
	{66, 18, 16}
};

static fwMaterial* particleMaterial = nullptr;

fwParticles::fwParticles(int nb, const std::string& sprite, float radius) : 
	fwSprites(nb)
{
	classID |= PARTICLES;

	m_positions = new glm::vec3[nb];
	m_velocities = new glm::vec3[nb];
	m_origins = new glm::vec3[nb];
	m_timer = new double[nb];
	m_active = new bool[nb];
	m_lifespan = new int[nb];

	m_colors = new glm::vec3[nb];

	for (auto i = 0; i < nb; i++) {
		m_active[i] = false;
	}

	m_image = new fwTexture(sprite);
	set(m_positions, m_image, radius);

	if (particleMaterial == nullptr) {
		particleMaterial = new fwMaterial("shaders/sprite/particle/vertex.glsl", "shaders/sprite/fragment.glsl");
		particleMaterial->addShader(FRAGMENT_SHADER, "shaders/sprite/fragment_defered.glsl", DEFERED_RENDER);
		m_material = particleMaterial;
	}

	m_geometry->addAttribute("aColor", GL_ARRAY_BUFFER, m_colors, 3, sizeof(glm::vec3) * m_size, sizeof(float));
}

void fwParticles::update_particle(int &spwanable, int i, double timer)
{
	glm::vec3* position = m_positions + i;
	glm::vec3* velocity = m_velocities + i;
	glm::vec3* origin = m_origins + i;
	double *time = m_timer + i;
	bool* active = m_active + i;
	int* lifespan= m_lifespan + i;
	glm::vec3* color = m_colors + i;

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
			*lifespan = 0;

			spwanable--;

			*origin = m_Position;
		}
		else {
			return;
		}
	}

	position->x = velocity->x * *time + origin->x;
	position->y = -0.7 * 9.81 * (*time * *time) + velocity->y * *time + origin->y;
	position->z = velocity->z * *time + origin->z;

	int color_index = *lifespan / 30;
	if (color_index > 11) {
		*active = false;
		spwanable++;
		return;
	}

	color->r = _steel_colors[color_index][0] / 255.0;
	color->g = _steel_colors[color_index][1] / 255.0;
	color->b = _steel_colors[color_index][2] / 255.0;

	*time += timer;
	(*lifespan)++;

	// despawn particle
	if (position->y < 0) {
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
	updateAttribute("aColor");
}

fwParticles::~fwParticles()
{
	delete[] m_positions;
	delete[] m_velocities;
	delete[] m_timer;
	delete[] m_active;
	delete[] m_lifespan;
	delete[] m_colors;
	delete[] m_origins;

	delete m_image;
}