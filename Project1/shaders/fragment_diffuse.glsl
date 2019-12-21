#version 330 core

#define DEFINES

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

// uniform vec4 ourColor;
in vec3 ourColor;
in vec2 TexCoord;
in vec3 normal;
in vec3 world;
#ifdef TEXTURE_ARRAY
	flat in float layer;
#endif

struct Material {
#ifdef TEXTURE_ARRAY
    sampler2DArray diffuse;
    sampler2DArray specular;
    sampler2DArray normalMap;
#else
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normalMap;
#endif
    float     shininess;
}; 

#include "include/camera.glsl"
#include "bloom/luminance.glsl"

uniform Material material;

#ifdef NORMALMAP
	in mat3 tbn;
#endif

#if DIRECTION_LIGHTS > 0
#include "lightning/directional.glsl"
#endif

#if POINT_LIGHTS > 0
#include "lightning/point.glsl"
#endif

#if SPOT_LIGHTS > 0
#include "lightning/spot.glsl"
#endif

void main()
{
	vec4 color;

#ifdef TEXTURE_ARRAY
	color = texture(material.diffuse, vec3(TexCoord, layer));
#else
	color = texture(material.diffuse, TexCoord);
#endif

    // diffuse 
    vec3 norm = normalize(normal);
#ifdef NORMALMAP
	norm = texture(material.normalMap, TexCoord).rgb;
	norm = normalize(norm * 2.0 - 1.0);   
	norm = normalize(tbn * norm); 
#endif

    vec3 viewDir = normalize(viewPos - world);

	vec3 dirlight;
	vec3 pointlight;
	vec3 spotlight;

	float specular_map = 1.0;
	#ifdef SPECULAR_MAP
		specular_map = texture(material.specular, TexCoord).r;  
	#endif

#if DIRECTION_LIGHTS > 0
    for(int i = 0; i < DIRECTION_LIGHTS; i++) {
#ifdef SHADOWMAP
			vec4 dirLight_world = dirlights[i].matrix * vec4(world, 1.0);
	        dirlight += CalcDirLight(i, norm, color.rgb, world, material.shininess, specular_map, viewDir, dirLight_world[i]);
#else
	        dirlight += CalcDirLight(i, norm, color.rgb, world, material.shininess, specular_map, viewDir, vec4(0));
#endif
	}
#endif

#if POINT_LIGHTS > 0
    for(int i = 0; i < POINT_LIGHTS; i++) {
		pointlight += CalcPointLight(i, norm, color.rgb, world, material.shininess, specular_map, viewDir);
	}
#endif

#if SPOT_LIGHTS > 0
    for(int i = 0; i < SPOT_LIGHTS; i++)
		spotlight += CalcSpotLight(i, norm, color.rgb, world, viewDir);
#endif

    vec3 result = clamp(dirlight + pointlight + spotlight, .0, 1.0);

	// color output
    FragColor = vec4(result, color.a);

	// bloom output
	float luminance = czm_luminance(color.rgb);
	if (luminance < 0.1) {
		BrightColor = color;
	}
	else {
		BrightColor = vec4 (0);
	}
}
