#version 330 core
out vec4 FragColor;

#include "../include/camera.glsl"

#define DEFINES

// from vextex
in vec2 TexCoord;

// GBuffer textures
uniform sampler2D gColor;
uniform sampler2D gNormal;
uniform sampler2D gWorld;
uniform sampler2D gMaterial;

struct DirectionlLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	mat4 matrix;
	sampler2D shadowMap;
};

uniform DirectionlLight dirlights[DIRECTION_LIGHTS];

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

#ifdef SHADOWMAP
in vec4 dirLight_world[DIRECTION_LIGHTS];
#endif

vec3 CalcDirLight(int i, vec3 normal, vec3 color, vec3 world, float shininess, float specular_map)
{
	// ambient
	vec3 ambient = dirlights[i].ambient;

	vec3 lightDir = normalize(dirlights[i].direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = dirlights[i].diffuse * diff;  
    
	// specular
	vec3 viewDir = normalize(viewPos - world);
	vec3 reflectDir = reflect(-lightDir, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	vec3 specular = dirlights[i].specular * spec * specular_map;  

	#ifdef SHADOWMAP
		// calculate shadow
		float shadow = ShadowCalculation(dirLight_world[i], light.shadowMap);                      
		return (ambient + (1.0 - shadow) * (diffuse + s)) * color;  
	#else
		return (ambient + diffuse + specular) * color;
	#endif
}

void main()
{
    // retrieve data from G-buffer
    vec3 world = texture(gWorld, TexCoord).rgb;
    vec3 normal = texture(gNormal, TexCoord).rgb;
    vec3 color = texture(gColor, TexCoord).rgb;
	vec3 material = texture(gMaterial, TexCoord).rgb;

    float shininess = material.r * 256;
    float specular_map = material.g;

    // diffuse 
    vec3 norm = normalize(normal);

    vec3 viewDir = normalize(viewPos - world);

	vec3 dirlight = vec3(0);

    for(int i = 0; i < DIRECTION_LIGHTS; i++)
        dirlight += CalcDirLight(i, normal, color, world, shininess, specular_map);

    FragColor = vec4(dirlight, 1.0);
}