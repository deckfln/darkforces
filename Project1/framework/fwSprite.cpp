#include "fwSprite.h"

#include "materials/fwSpriteMaterial.h"

fwSpriteMaterial SpriteMaterial; 

fwSprite::fwSprite(glm::vec3 *position, int nb, fwTexture *texture)
{
	classID |= SPRITE;

	geometry = new fwGeometry();
	geometry->addVertices("aPos", position, 3, sizeof(glm::vec3) * nb, sizeof(float));

	material = new fwSpriteMaterial();
	material->addTexture("texture", texture);
}

void fwSprite::draw(glProgram *program)
{
	// create one VAO by shader class

	GLuint id = program->getID();
	if (vao.count(id) == 0) {
		vao[id] = new glVertexArray();
		geometry->enable_attributes(program);
		vao[id]->unbind();
	}

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	geometry->draw(GL_POINTS, vao[id]);
}

fwSprite::~fwSprite()
{
	delete geometry;
}