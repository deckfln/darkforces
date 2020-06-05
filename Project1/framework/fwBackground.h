#pragma once

#include <string>
#include <list>

class glProgram;
class glVertexArray;
class fwGeometry;
class fwUniform;
class fwCamera;

class fwBackground
{
protected:
	glProgram* m_program = nullptr;
	glVertexArray* m_cube = nullptr;
	fwGeometry* m_geometry = nullptr;

	bool m_uploaded = false;	// did we upload all uniforms ?
	std::list<fwUniform*> m_uniforms;

	std::string get_shader(const std::string shader_file);

public:
	fwBackground(std::string vertexs, std::string fragments);
	void draw(fwCamera* camera, int renderMode);
	~fwBackground();
};