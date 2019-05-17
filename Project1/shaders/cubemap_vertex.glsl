#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140) uniform Camera
{
	mat4 view;
	mat4 projection;
	vec3 viewPos;
};

void main()
{
    TexCoords = aPos;
	mat4 remove_translation = mat4(mat3(view));
    vec4 pos = projection * remove_translation * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
} 