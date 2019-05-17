#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 normal;
out vec3 world;

uniform mat4 model;

layout (std140) uniform Camera
{
	mat4 view;
	mat4 projection;
};

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = aTexCoord;
	world = vec3(model * vec4(aPos, 1.0));
	normal = mat3(transpose(inverse(model))) * aNormal; 
}
