#version 330 core
#define DEFINES

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

#ifdef INSTANCED
layout (location = 2) in mat4 model;
#else
uniform mat4 model;
#endif

#include "include/camera.glsl"

void main()
{
	// move outward the vertice unlong the normal
	vec3 pos = aPos + aNormal * 0.02;

	gl_Position = projection * view * model * vec4(pos, 1.0);
}
