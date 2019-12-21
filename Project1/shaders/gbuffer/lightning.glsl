#version 330 core
#define DEFINES

layout (location = 0) out vec4 FragColor;

#include "../include/camera.glsl"

#ifdef BLOOMMAP
layout (location = 1) out vec4 BrightColor;
#include "../bloom/luminance.glsl"
#endif


// from vextex
in vec2 TexCoord;

// GBuffer textures
uniform sampler2D gColor;
uniform sampler2D gNormal;
uniform sampler2D gWorld;
uniform sampler2D gMaterial;

#if DIRECTION_LIGHTS > 0
#include "../lightning/directional.glsl"
#endif

#if POINT_LIGHTS > 0
#include "../lightning/point.glsl"
#endif

void main()
{
    // retrieve data from G-buffer
    vec3 color = texture(gColor, TexCoord).rgb;
    vec3 normal = texture(gNormal, TexCoord).rgb;

	if (normal == vec3(0)) {
		// basic material
		FragColor = vec4(color, 1.0);
	}
	else {
		// diffuse material
		vec3 world = texture(gWorld, TexCoord).rgb;
		vec3 material = texture(gMaterial, TexCoord).rgb;

		float shininess = material.r * 256;
		float specular_map = material.g;

		// diffuse 
		vec3 norm = normalize(normal);

		vec3 viewDir = normalize(viewPos - world);

		vec3 dirlight = vec3(0);

#if DIRECTION_LIGHTS > 0
		for(int i = 0; i < DIRECTION_LIGHTS; i++) {
#ifdef SHADOWMAP
			vec4 dirLight_world = dirlights[i].matrix * vec4(world, 1.0);
			dirlight += CalcDirLight(i, normal, color, world, shininess, specular_map, viewDir, dirLight_world);
#else
			dirlight += CalcDirLight(i, normal, color, world, shininess, specular_map, viewDir, vec4(0));
#endif
		}
#endif

#if POINT_LIGHTS > 0
	    for(int i = 0; i < POINT_LIGHTS; i++)
			pointlight += CalcPointLight(i, norm, color, world, shininess, specular_map, viewDir);
		}
#endif
		FragColor = vec4(dirlight, 1.0);

	}

#ifdef BLOOMMAP
	// bloom output
	float luminance = czm_luminance(color.rgb);
	if (luminance > 0 && luminance < 0.1) {
		BrightColor = FragColor;
	}
	else {
		BrightColor = vec4 (0);
	}
#endif
}