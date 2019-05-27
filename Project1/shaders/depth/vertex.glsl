
#version 330 core
layout (location = 0) in vec3 aPos;

#include "../include/camera.glsl"

#define DEFINES

#ifdef INSTANCED
layout (location = 4) in mat4 model;
#else
uniform mat4 model;
#endif

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}  