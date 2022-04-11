#pragma once

#include <glm/vec4.hpp>
#include "../framework/fwMesh2D.h"

class fwTexture;

namespace GameEngine 
{
	class Image2D : public Framework::Mesh2D 
	{
		fwTexture* m_texture = nullptr;
		std::vector<fwUniform*> m_uniforms;
		void setGeometry(void);
		void setMaterial(void);
		void setUniforms(void);

	public:
		Image2D(const std::string& name,
			const glm::vec2& scale,
			const glm::vec2& translate,
			fwTexture* texture = nullptr,
			fwMaterial* material = nullptr
		);

		// create with a shader
		Image2D(const std::string& name,
			const glm::vec2& scale,
			const glm::vec2& translate,
			fwTexture* texture,
			const std::map<ShaderType, std::string>& shaders
		);

		void setTexture(fwTexture*);
	};
}