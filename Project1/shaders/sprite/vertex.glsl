#version 330 core

layout (location = 0) in vec3 aPos;

#include "../include/camera.glsl"

uniform mat4 model;

void main()
{
	gl_Position = projection * view * vec4(aPos, 1.0);
}
