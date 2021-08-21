#version 330 core
layout (location = 0) out vec3 gFragColor;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gWorld;
layout (location = 3) out vec3 gMaterial;

// uniform vec4 ourColor;
in vec3 ourColor;
in vec2 TexCoord;
in vec3 normal;
in vec3 world;

#define DEFINES

struct Material {
	vec4 color;

	#ifdef DIFFUSE_MAP
    sampler2D diffuse;
	#endif
};

#include "../include/camera.glsl"

#ifdef COLORS
	in vec3 vcolor;
#else
	uniform Material material;
#endif

void main()
{
	vec4 color;

#ifdef DIFFUSE_MAP
	color = texture(material.diffuse, TexCoord);
#else
	#ifdef COLORS
		color = vec4(vcolor, 1.0);
	#else
		color = material.color;
	#endif
#endif

    gFragColor = color.rgb;
	gWorld = world;
	gNormal = vec3(0);
	gMaterial = vec3(0);
}