#version 330 core
#define DEFINES

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

#ifdef INSTANCED
layout (location = 4) in mat4 model;
#else
uniform mat4 model;
#endif

out vec3 ourColor;
out vec2 TexCoord;
out vec3 normal;
out vec3 world;

#ifdef SHADOWMAP
#if DIRECTION_LIGHTS > 0
	struct DirectionlLight {
		vec3 direction;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;

		mat4 matrix;
		sampler2D shadowMap;
	};
	uniform DirectionlLight dirlights[DIRECTION_LIGHTS];
	out vec4 dirLight_world[DIRECTION_LIGHTS];
#endif
#endif

#include "include/camera.glsl"

void main()
{
	world = vec3(model * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoord = aTexCoord;
 
#ifdef SHADOWMAP
#if DIRECTION_LIGHTS > 0
	for (int i=0; i<DIRECTION_LIGHTS; i++) {
		dirLight_world[i] = dirlights[i].matrix * vec4(world, 1.0);
	}
#endif
#endif

    gl_Position = projection * view * vec4(world, 1.0);
}