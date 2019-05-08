#version 330 core
out vec4 FragColor;
// uniform vec4 ourColor;
in vec3 ourColor;
in vec2 TexCoord;
in vec3 normal;
in vec3 world;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
}; 

uniform vec3 viewPos;
uniform Material material;

#define DEFINES

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

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 diffuse, vec3 world, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - world);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - world);
    float attenuation = 1.0 / (light.constant + light.linear * distance +  light.quadratic * (distance * distance));    

    // combine results
    vec3 ambient  = light.ambient * diffuse;
    vec3 ndiffuse  = light.diffuse * diff * diffuse;

	#ifdef SPECULAR_MAP
	    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
	#else
		vec3 specular = light.specular * spec * vec3(1.0);
	#endif

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

	return ambient + ndiffuse + specular;
}

#endif

#if DIRECTION_LIGHTS > 0
struct DirectionlLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionlLight DirLights[DIRECTION_LIGHTS];

vec3 CalcDirLight(DirectionlLight light, vec3 normal, vec3 diffuse)
{
    // ambient
    vec3 ambient = light.ambient * diffuse;

    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 ndiffuse = light.diffuse * diff * diffuse;  
    
    // specular
    vec3 viewDir = normalize(viewPos - world);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	#ifdef SPECULAR_MAP
	    vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;  
	#else
		vec3 specular = light.specular * spec * vec3(1.0);
	#endif

	return ambient + ndiffuse + specular;
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
	vec3 diffuse = texture(material.diffuse, TexCoord).rgb;

    // diffuse 
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - world);

	vec3 dirlight;
	vec3 pointlight;
	vec3 spotlight;

#if DIRECTION_LIGHTS > 0
    for(int i = 0; i < DIRECTION_LIGHTS; i++)
        dirlight += CalcDirLight(dirlights[i], norm);
#endif

#if POINT_LIGHTS > 0
    for(int i = 0; i < POINT_LIGHTS; i++)
		pointlight += CalcPointLight(pointlights[i], norm, diffuse, world, viewDir);
#endif

#if SPOT_LIGHTS > 0
    for(int i = 0; i < SPOT_LIGHTS; i++)
		spotlight += CalcSpotLight(spotlights[i], norm, diffuse, world, viewDir);
#endif

    vec3 result = clamp(dirlight + pointlight + spotlight, .0, 1.0);

    FragColor = vec4(result, 1.0);
}