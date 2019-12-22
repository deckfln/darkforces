//
// Point Lights
//

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float radius;
	#ifdef SHADOWMAP
		float far_plane;
		samplerCube shadowMap;
	#endif
};

uniform PointLight pointlights[POINT_LIGHTS];

#ifdef SHADOWMAP
	float CalcPointLightShadow(int i, vec3 fragPos)
	{
		// get vector between fragment position and light position
		vec3 fragToLight = fragPos - pointlights[i].position;
		// use the light to fragment vector to sample from the depth map    
		float closestDepth = texture(pointlights[i].shadowMap, fragToLight).r;
		// it is currently in linear range between [0,1]. Re-transform back to original value
		closestDepth *= pointlights[i].far_plane;
		// now get current linear depth as the length between the fragment and light position
		float currentDepth = length(fragToLight);
		// now test for shadows
		float bias = 0.05; 
		float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

		return shadow;
	}  
#endif

vec3 CalcPointLight(int i, vec3 normal, vec3 color, vec3 world, float shininess, float specular_map, vec3 viewDir)
{

	vec3 lightDir = pointlights[i].position - world;
	float distance    = length(lightDir);
	if (distance > pointlights[i].radius) {
		return vec3(0);
	}

	// attenuation
	float attenuation = 1.0 / (pointlights[i].constant + pointlights[i].linear * distance +  pointlights[i].quadratic * (distance * distance));    

	lightDir = normalize(lightDir);

	// diffuse shading
	float diff = max(dot(lightDir, normal), 0.0);

	// specular shading

	// blinn-phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

	// phong
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// combine results
	vec3 ambient  = pointlights[i].ambient * color;
	vec3 diffuse  = pointlights[i].diffuse * diff * color;

	#ifdef SPECULAR_MAP
		vec3 specular = pointlights[i].specular * spec * specular_map;
	#else
		vec3 specular = pointlights[i].specular * spec;
	#endif
	
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	
	#ifdef SHADOWMAP
		// calculate shadow
		float shadow = CalcPointLightShadow(i, world);
		return (ambient + (1.0 - shadow) * (diffuse + specular)) * color; 
	#else
		return (ambient + diffuse + specular) * color;
	#endif

	return ambient + diffuse + specular;
}
