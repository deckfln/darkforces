#pragma once
#include <iostream>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "../glad/glad.h"

class glUniformBuffer;
class glProgram;
class glTexture;
class glTextureArray;
class glCubeTexture;
class glUniformBuffer;

class fwUniform
{
	std::string m_name;
	void *m_data = nullptr;
	int m_type = -1;
	int m_size = 0;	// if this is an array of value

public:
	fwUniform();
	fwUniform(const std::string& _name, glm::vec4*_v4);
	fwUniform(const std::string& _name, glm::vec3* v3);
	fwUniform(const std::string& _name, glm::vec2* v2);
	fwUniform(const std::string& _name, GLint *id);
	fwUniform(const std::string& _name, GLfloat *f);
	fwUniform(const std::string& _name, glTexture *y);
	fwUniform(const std::string& _name, glTextureArray* y);
	fwUniform(const std::string& _name, glCubeTexture *y);
	fwUniform(const std::string& _name, glm::mat4* t, int size);
	fwUniform(const std::string& _name, glm::vec4* t, int size);
	fwUniform(const std::string& _name, glUniformBuffer *ubo);
	void set_uniform(glProgram *);
	void *get(void);

	void set(void *_data);
	void set(const std::string& _name, glm::vec4* _v4);
	void set(const std::string& _name, glm::vec2* _v2);
	void set(const std::string& _name, GLint* id);
	void set(const std::string& _name, GLfloat* f);
	void set(const std::string& _name, glTexture* y);
	void set(const std::string& _name, glTextureArray* y);
	void set(const std::string& _name, glCubeTexture* y);
	void set(const std::string& _name, int32_t* t, int size);
	void set(const std::string& _name, glm::mat4* t, int size);
	void set(const std::string& _name, glm::vec3* t, int size);
	void set(const std::string& _name, glm::vec4* t, int size);
	void set(const std::string& _name, glUniformBuffer* ubo);

	inline const std::string& name(void) { return m_name; };
};
