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

vec3 CalcSpotLight(int i, vec3 normal, vec3 diffuse, vec3 world, vec3 viewDir)
{
	vec3 ambient  = spotlights[i].ambient * diffuse;

	// diffuse 
	vec3 lightDir = normalize(spotlights[i].position - world);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 ndiffuse = spotlights[i].diffuse * diff * diffuse;  
    
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	#ifdef SPECULAR_MAP
		vec3 specular = spotlights[i].specular * spec * texture(material.specular, TexCoord).rgb;  
	#else
		vec3 specular = spotlights[i].specular * spec * vec3(1.0);
	#endif

    
	// spotlight (soft edges)
	float theta = dot(lightDir, normalize(-spotlights[i].direction)); 
	float epsilon = (light.cutOff - spotlights[i].outerCutOff);
	float intensity = clamp((theta - spotlights[i].outerCutOff) / epsilon, 0.0, 1.0);
	diffuse  *= intensity;
	specular *= intensity;
    
	// attenuation
	float distance    = length(spotlights[i].position - world);
	float attenuation = 1.0 / (light.constant + spotlights[i].linear * distance + spotlights[i].quadratic * (distance * distance));    
	ambient  *= attenuation; 
	diffuse   *= attenuation;
	specular *= attenuation;

	return ambient + ndiffuse + specular;
}
