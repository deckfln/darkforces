#pragma once

#include <string>

class glProgram;
class glVertexArray;
class fwGeometry;
class fwUniform;
class fwCamera;

class fwBackground
{
protected:
	int id;
	glProgram* program = nullptr;
	glVertexArray* cube = nullptr;
	fwGeometry* geometry = nullptr;
	fwUniform* uniform = nullptr;

	std::string get_shader(const std::string shader_file);

public:
	fwBackground(std::string vertexs, std::string fragments);
	void draw(fwCamera* camera, int renderMode);
	virtual void setUniforms(glProgram*) {};
	~fwBackground();
};