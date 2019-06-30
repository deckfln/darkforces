#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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

#include "include/camera.glsl"

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

#if POINT_LIGHTS > 0
	struct PointLight {
		vec3 position;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;

		float constant;
		float linear;
		float quadratic;
	};

	uniform PointLight pointlights[POINT_LIGHTS];

	vec3 CalcPointLight(PointLight light, vec3 normal, vec3 color, vec3 world, vec3 viewDir)
	{
		vec3 lightDir = normalize(light.position - world);

		// attenuation
		float distance    = length(light.position - world);
		float attenuation = 1.0 / (light.constant + light.linear * distance +  light.quadratic * (distance * distance));    

		// diffuse shading
		float diff = max(dot(lightDir, normal), 0.0);

		// specular shading

		// blinn-phong
		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

		// phong
		//vec3 reflectDir = reflect(-lightDir, normal);
		//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

		// combine results
		vec3 ambient  = light.ambient * color;
		vec3 diffuse  = light.diffuse * diff * color;

		#ifdef SPECULAR_MAP
			vec3 specular = light.specular * spec * texture(material.specular, TexCoord).r;
		#else
			vec3 specular = light.specular * spec;
		#endif
	
		ambient  *= attenuation;
		diffuse  *= attenuation;
		specular *= attenuation;
	
		return ambient + diffuse + specular;
	}
#endif

#if DIRECTION_LIGHTS > 0
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
	in vec4 dirLight_world[DIRECTION_LIGHTS];
	#endif

	vec3 CalcDirLight(DirectionlLight light, int i, vec3 normal, vec3 color)
	{
		// ambient
		vec3 ambient = light.ambient;

		vec3 lightDir = normalize(light.direction);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = light.diffuse * diff;  
    
		// specular
		vec3 viewDir = normalize(viewPos - world);
		vec3 reflectDir = reflect(-lightDir, normal);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

		#ifdef SPECULAR_MAP
			vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;  
		#else
			vec3 specular = light.specular * spec;
		#endif

		#ifdef SHADOWMAP
			// calculate shadow
			float shadow = ShadowCalculation(dirLight_world[i], light.shadowMap);                      
			return (ambient + (1.0 - shadow) * (diffuse + specular)) * color;  
		#else
			return (ambient + diffuse + specular) * color;
		#endif
	}
#endif

#if SPOT_LIGHTS > 0
	struct SpotLight {
		vec3 position;  
		vec3 direction;
		float cutOff;
		float outerCutOff;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
	
		float constant;
		float linear;
		float quadratic;
	};

	uniform SpotLight spotlights[SPOT_LIGHTS];

	vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 diffuse, vec3 world, vec3 viewDir)
	{
		vec3 ambient  = light.ambient * diffuse;

		// diffuse 
		vec3 lightDir = normalize(light.position - world);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 ndiffuse = light.diffuse * diff * diffuse;  
    
		// specular
		vec3 reflectDir = reflect(-lightDir, normal);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

		#ifdef SPECULAR_MAP
			vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;  
		#else
			vec3 specular = light.specular * spec * vec3(1.0);
		#endif

    
		// spotlight (soft edges)
		float theta = dot(lightDir, normalize(-light.direction)); 
		float epsilon = (light.cutOff - light.outerCutOff);
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
		diffuse  *= intensity;
		specular *= intensity;
    
		// attenuation
		float distance    = length(light.position - world);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
		ambient  *= attenuation; 
		diffuse   *= attenuation;
		specular *= attenuation;

		return ambient + ndiffuse + specular;
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

    vec3 viewDir = normalize(viewPos - world);

	vec3 dirlight;
	vec3 pointlight;
	vec3 spotlight;

#if DIRECTION_LIGHTS > 0
    for(int i = 0; i < DIRECTION_LIGHTS; i++)
        dirlight += CalcDirLight(dirlights[i], i, norm, color.rgb);
#endif

#if POINT_LIGHTS > 0
    for(int i = 0; i < POINT_LIGHTS; i++)
		pointlight += CalcPointLight(pointlights[i], norm, color.rgb, world, viewDir);
#endif

#if SPOT_LIGHTS > 0
    for(int i = 0; i < SPOT_LIGHTS; i++)
		spotlight += CalcSpotLight(spotlights[i], norm, color.rgb, world, viewDir);
#endif

    vec3 result = clamp(dirlight + pointlight + spotlight, .0, 1.0);

	// color output
    FragColor = vec4(result, color.a);

	// bloom output
	float luminance = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	if (luminance > 0.8) {
		BrightColor = color;
	}
	else {
		BrightColor = vec4 (0.0, 0.0, 0.0, 0.0);
	}
}
