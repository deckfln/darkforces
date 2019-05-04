#version 330 core
out vec4 FragColor;
// uniform vec4 ourColor;
in vec3 ourColor;
in vec2 TexCoord;
in vec3 normal;
in vec3 world;

uniform sampler2D ourTexture;
uniform sampler2D texture1;
uniform vec4 ambientColor;
uniform vec3 lightPos;
uniform vec4 lightColor;
uniform vec3 viewPos;

void main()
{
	vec4 objectColor = mix(texture(ourTexture, TexCoord), texture(texture1, TexCoord), 0.3);

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - world);  

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor.rgb;

	vec3 ambient = ambientColor.rgb * ambientColor.a;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - world);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStrength * spec * lightColor.rgb;

	FragColor = objectColor * vec4(ambient + diffuse + specular, 1.0);
}