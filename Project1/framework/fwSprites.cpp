#include "fwSprites.h"

static fwMaterial* spriteMaterial = nullptr;

fwSprites::fwSprites(int size) :
	m_size(size)
{
	classID |= SPRITE;
}

fwSprites::fwSprites(glm::vec3 *position, int nb, fwTexture *texture, float radius) : 
	m_size(nb)
{
	classID |= SPRITE;
	set(position, texture, radius);
}

void fwSprites::set(glm::vec3* position, fwTexture* texture, float radius)
{
	geometry = new fwGeometry();
	geometry->addVertices("aPos", position, 3, sizeof(glm::vec3) * m_size, sizeof(float));
	geometry->setBoundingsphere(radius);

	if (spriteMaterial == nullptr) {
		spriteMaterial = new fwMaterial("shaders/sprite/vertex.glsl", "shaders/sprite/fragment.glsl");
		spriteMaterial->addShader(FRAGMENT_SHADER, "shaders/sprite/fragment_defered.glsl", DEFERED_RENDER);
	}
	material = spriteMaterial;
	material->addTexture("texture", texture);
}

void fwSprites::draw(glProgram *program)
{
	// create one VAO by shader class
	GLuint id = buildVAO(program);

	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SMOOTH);

	geometry->draw(GL_POINTS, vao[id]);

	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_PROGRAM_POINT_SIZE);
}

fwSprites::~fwSprites()
{
	delete geometry;
}