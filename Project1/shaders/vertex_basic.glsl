#version 330 core
#define DEFINES

layout (location = 0) in vec3 aPos;

out vec3 world;

#ifdef DIFFUSE_MAP
	layout (location = 1) in vec2 aTexCoord;
	out vec2 TexCoord;
#endif

uniform mat4 model;

#include "include/camera.glsl"

void main()
{
#ifdef DIFFUSE_MAP
    TexCoord = aTexCoord;
#endif
	world = vec3(model * vec4(aPos, 1.0));

	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
