#version 330 core

#define DEFINES

layout (location = 0) out vec3 gFragColor;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gWorld;
layout (location = 3) out vec3 gMaterial;

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

#include "../include/camera.glsl"

uniform Material material;

#ifdef NORMALMAP
	in mat3 tbn;
#endif

void main()
{
	vec4 color;

#ifdef TEXTURE_ARRAY
	color = texture(material.diffuse, vec3(TexCoord.x, TexCoord.y, layer));
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

	float shininess = material.shininess / 256;
	float specular = 1.0;

	#ifdef SPECULAR_MAP
		specular = texture(material.specular, TexCoord).r;  
	#endif

    gFragColor = color.rgb;
	gWorld = world;
	gNormal = norm;
	gMaterial = vec3(shininess, specular, 1.0);
}