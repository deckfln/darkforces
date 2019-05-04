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

#define POINT_LIGHT
struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform vec3 viewPos;
uniform Material material;

#ifdef POINT_LIGHT
uniform PointLight light;
#endif
#ifdef DIRECTION_LIGHT
uniform DirectionlLight light;
#endif

void main()
{
	vec3 diffuse = texture(material.diffuse, TexCoord).rgb;
    // ambient
    vec3 ambient = light.ambient * diffuse;
  	
    // diffuse 
    vec3 norm = normalize(normal);

#ifdef POINT_LIGHT
    vec3 lightDir = normalize(light.position - world);
	#endif
#ifdef DIRECTION_LIGHT
    vec3 lightDir = normalize(light.direction);
#endif

    float diff = max(dot(norm, lightDir), 0.0);
    diffuse *= light.diffuse * diff;  
    
    // specular
    vec3 viewDir = normalize(viewPos - world);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;  
        
#ifdef POINT_LIGHT
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