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
flat in uint textureID;	// index start in megatexture
flat in float ambient;

#define DEFINES

struct Material {
	vec4 color;

	#ifdef DIFFUSE_MAP
    sampler2D diffuse;
	#endif
};

#include "../../shaders/include/camera.glsl"

uniform Material material;
uniform vec4 megatexture_idx[128];

void main()
{
	vec4 color;

#ifdef DIFFUSE_MAP
	vec2 uv = fract(TexCoord);

	// convert the local texture UV into the MegaTexture uv
	vec4 textureData = megatexture_idx[textureID];

	vec2 mega_uv = textureData.rg + textureData.ba * uv;
	color = texture(material.diffuse, mega_uv);
#else
	color = material.color;
#endif

    gFragColor = color.rgb;
	gWorld = world;
	gNormal = vec3(0);

	// record approx depth for the headlight. the headlight cannot light over 5 units
	float z = gl_FragCoord.z / gl_FragCoord.w;
	gMaterial = vec3(0.0, z / 2.5f, ambient);
}