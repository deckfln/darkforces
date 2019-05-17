#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;

layout (std140) uniform Camera
{
	mat4 view;
	mat4 projection;
	vec3 viewPos;
};

void main()
{
	// move outward the vertice unlong the normal
	vec3 pos = aPos + aNormal * 0.02;

	gl_Position = projection * view * model * vec4(pos, 1.0);
}
