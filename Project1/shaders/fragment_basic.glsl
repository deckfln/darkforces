#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#define DEFINES

#ifdef DIFFUSE_MAP
	in vec2 TexCoord;
#endif

struct Material {
	vec4 color;
	#ifdef DIFFUSE_MAP
    sampler2D diffuse;
	#endif
};

uniform Material material;

#include "bloom/luminance.glsl"

void main()
{
	vec4 color = vec4(0);
#ifdef DIFFUSE_MAP
    color  = texture(material.diffuse, TexCoord);
#else
    color  = material.color;
#endif

	// bloom output
	float luminance = czm_luminance(color.rgb);
	if (luminance > 0 && luminance < 0.1) {
		BrightColor = color;
	}
	else {
		BrightColor = vec4 (0);
	}

	FragColor = color;
}