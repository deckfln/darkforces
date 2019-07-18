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

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normalMap;
    float     shininess;
}; 

#include "../include/camera.glsl"

uniform Material material;

#define DEFINES

#ifdef NORMALMAP
	in mat3 tbn;
#endif

void main()
{
	vec4 color = texture(material.diffuse, TexCoord);

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
	gNormal = norm;
	gWorld = world;
	gMaterial = vec3(shininess, specular, 1.0);
}