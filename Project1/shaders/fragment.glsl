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

void main()
{
	vec4 objectColor = mix(texture(ourTexture, TexCoord), texture(texture1, TexCoord), 0.2);

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - world);  

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor.rgb;

	vec3 ambient = ambientColor.rgb * ambientColor.a;

	FragColor = objectColor * vec4(ambient + diffuse, 1.0);
}