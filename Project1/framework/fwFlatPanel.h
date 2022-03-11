#pragma once

/**
 * Draw a flat quad on screen outside of the renderer loop with a dedicated material
 */

#include <string>
#include <glm/vec4.hpp>

class glProgram;
class glVertexArray;
class fwMaterial;
class fwTexture;
class fwGeometry;

class fwFlatPanel
{
	fwMaterial* m_material = nullptr;
	glProgram* m_program = nullptr;
	fwGeometry* m_geometry = nullptr;
	glVertexArray* m_vertexArray = nullptr;

public:
	fwFlatPanel(fwMaterial* material);
	void set(const std::string& name, fwTexture* texture);
	void set(const std::string& name, glm::vec4* v4);
	void draw(void);
	~fwFlatPanel();
};