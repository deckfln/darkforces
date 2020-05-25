#version 330 core
#define DEFINES

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aTex;

#ifdef INSTANCED
layout (location = 4) in mat4 model;
#else
uniform mat4 model;
#endif

out vec3 ourColor;
out vec3 world;
out vec2 uv;
flat out int textureID;

#include "../../../shaders/include/camera.glsl"

void main()
{
	world = vec3(model * vec4(aPos, 1.0));
	ourColor = aColor;
	uv = aTex.xy;
	textureID = int(aTex.z);

    gl_Position = projection * view * vec4(world, 1.0);
}