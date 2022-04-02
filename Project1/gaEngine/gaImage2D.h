#pragma once

#include <glm/vec4.hpp>
#include "../framework/fwMesh2D.h"

namespace GameEngine 
{
	class Image2D : public Framework::Mesh2D 
	{
		glm::vec4 m_onscreen = glm::vec4(0);
		fwTexture* m_texture = nullptr;
		std::vector<fwUniform*> m_uniforms;

	public:
		// create with a material
		Image2D(const std::string& name,
			float x, float y, 
			float width, float height,
			fwTexture* texture, 
			fwMaterial* material=nullptr);

		// create with a shader
		Image2D(const std::string& name,
			float x, float y,
			float width, float height,
			fwTexture* texture,
			const std::map<ShaderType, std::string>& shaders
		);
	};
}