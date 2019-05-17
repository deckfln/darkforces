layout (std140) uniform Camera
{
	mat4 view;
	mat4 projection;
	vec3 viewPos;
};