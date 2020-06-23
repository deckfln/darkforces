#include "fwInstancedMesh.h"
#include "../glad/glad.h"
#include "../glEngine/glInstancedVertexArray.h"

fwInstancedMesh::fwInstancedMesh(fwGeometry *_geometry, fwMaterial *_material, int _instances, glm::mat4 *_positions):
	fwMesh(_geometry, _material),
	max_instances(_instances),
	instances_to_draw(_instances)
{
	// glm::vec3 = 3 * sizeof(float)
	positions = new glInstancedBufferAttribute("_positions", 1, GL_ARRAY_BUFFER, _positions, 16, max_instances * sizeof(glm::mat4), sizeof(float), false);
	classID |= INSTANCED_MESH;
}

void fwInstancedMesh::set_instances_2_draw(int _instances)
{
	instances_to_draw = _instances;
	dirty_instances = true;
}

void fwInstancedMesh::update_position(int start, int count)
{
	positions->update(start, count);
}

void fwInstancedMesh::draw(glProgram *program)
{
	// create one VAO by shader class

	GLuint id = program->getID();
	if (vao.count(id) == 0) {
		vao[id] = new glInstancedVertexArray(instances_to_draw);
		geometry->enable_attributes(program);
		glVertexAttribute *va = program->get_attribute("model");
		if (va) {
			va->EnableVertex(positions);
		}
		vao[id]->unbind();
		dirty_instances = false;
	}
	else {
		if (dirty_instances) {
			glInstancedVertexArray *va = (glInstancedVertexArray *)vao[id];
			va->set_instances(instances_to_draw);
			dirty_instances = false;
		}
	}

	switch (m_rendering) {
	case fwMeshRendering::FW_MESH_POINT:
		glPointSize(m_pointSize);
		geometry->draw(GL_POINTS, vao[id]);
		glPointSize(1.0f);
		break;
	case fwMeshRendering::FW_MESH_LINES:
		geometry->draw(GL_LINES, vao[id]);
		break;
	case fwMeshRendering::FW_MESH_LINE:
		glEnable(GL_LINE_WIDTH);
		glLineWidth(16.0f);
		geometry->draw(GL_LINE, vao[id]);
		glDisable(GL_LINE_WIDTH);
		break;
	default:
		geometry->draw(GL_TRIANGLES, vao[id]);
		break;
	}
}

fwInstancedMesh::~fwInstancedMesh()
{
	delete positions;
}