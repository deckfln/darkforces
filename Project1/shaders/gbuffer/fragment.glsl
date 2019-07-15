#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec3 World;

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

#ifdef SHADOWMAP
	float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap)
	{
		// perform perspective divide
		vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
		// transform to [0,1] range
		projCoords = projCoords * 0.5 + 0.5;

		// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
		float closestDepth = texture(shadowMap, projCoords.xy).r; 
		// get depth of current fragment from light's perspective
		float currentDepth = projCoords.z;
		// check whether current frag pos is in shadow
		float bias = 0.005;
		float shadow = 0.0; 

		if(projCoords.z <= 1.0) {
			// PCF
			vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
			for(int x = -1; x <= 1; ++x) {
				for(int y = -1; y <= 1; ++y) {
					float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
					shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
				}    
			}
			shadow /= 9.0;		
		}

		return shadow;
	}  
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

	#ifdef SPECULAR_MAP
		float specular = texture(material.specular, TexCoord).r;  
	#else
		float specular = material.shininess;
	#endif

    FragColor = vec4(color.rgb, specular);
	Normal = norm;
	World = world;
}