#include "fwInstancedMesh.h"
#include "glad/glad.h"
#include "../glEngine/glInstancedVertexArray.h"

fwInstancedMesh::fwInstancedMesh(fwGeometry *_geometry, fwMaterial *_material, int _instances, glm::mat4 *_positions):
	fwMesh(_geometry, _material),
	instances(_instances)
{
	// glm::vec3 = 3 * sizeof(float)
	positions = new glInstancedBufferAttribute("_positions", 1, GL_ARRAY_BUFFER, _positions, 3, instances * sizeof(glm::mat4), sizeof(float), false);
	classID |= INSTANCED_MESH;
}

void fwInstancedMesh::draw(glProgram *program)
{
	// create one VAO by shader class

	GLuint id = program->getID();
	if (vao.count(id) == 0) {
		vao[id] = new glInstancedVertexArray(instances);
		geometry->enable_attributes(program);
		glVertexAttribute *va = program->get_attribute("model");
		if (va) {
			va->EnableVertex(positions);
		}
		vao[id]->unbind();
	}

	geometry->draw(wireFrame ? GL_LINES : GL_TRIANGLES, vao[id]);
}

fwInstancedMesh::~fwInstancedMesh()
{
	delete positions;
}