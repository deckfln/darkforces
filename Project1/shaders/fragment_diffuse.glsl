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
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionlLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

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

uniform vec3 viewPos;
uniform Material material;

#define SPOT_LIGHT

#ifdef POINT_LIGHT
uniform PointLight light;
#endif
#ifdef DIRECTION_LIGHT
uniform DirectionlLight light;
#endif
#ifdef SPOT_LIGHT
uniform SpotLight light;
#endif

void main()
{
	vec3 diffuse = texture(material.diffuse, TexCoord).rgb;

    // ambient
    vec3 ambient = light.ambient * diffuse;
  	vec3 specular = vec3(0);

    // diffuse 
    vec3 norm = normalize(normal);

#ifdef DIRECTION_LIGHT
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    diffuse *= light.diffuse * diff;  
    
    // specular
    vec3 viewDir = normalize(viewPos - world);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    specular = light.specular * spec * texture(material.specular, TexCoord).rgb;  
#endif

#ifdef POINT_LIGHT
    vec3 lightDir = normalize(light.position - world);
    float diff = max(dot(norm, lightDir), 0.0);
    diffuse *= light.diffuse * diff;  
    
    // specular
    vec3 viewDir = normalize(viewPos - world);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    specular = light.specular * spec * texture(material.specular, TexCoord).rgb;  

    // attenuation
    float distance    = length(light.position - world);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation; 
#endif

#ifdef SPOT_LIGHT
    // diffuse 
    vec3 lightDir = normalize(light.position - world);
    float diff = max(dot(norm, lightDir), 0.0);
    diffuse *= light.diffuse * diff;  
    
    // specular
    vec3 viewDir = normalize(viewPos - world);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    specular = light.specular * spec * texture(material.specular, TexCoord).rgb;  
    
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
#endif

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}