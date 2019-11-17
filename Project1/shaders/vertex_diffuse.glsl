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

#ifdef SKINNED
	layout (location = 5) in ivec4 bonesID;
	layout (location = 6) in vec4 bonesWeight;
	uniform mat4 gBones[64];
#endif

out vec3 ourColor;
out vec2 TexCoord;
out vec3 normal;
out vec3 world;

#ifdef NORMALMAP
	layout (location = 7) in vec3 tangent;
	out mat3 tbn;

	void computeTBN(mat3 normalMatrix, vec3 normal, vec3 tangent)
	{
		vec3 T = normalize(normalMatrix * tangent);
		vec3 N = normalize(normalMatrix * aNormal);
		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);
		tbn = mat3(T, B, N);
	}
#endif

#ifdef TEXTURE_ARRAY
	layout (location = 8) in int aLayer;	// target layer for texture arrays
	out int layer;
#endif

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
	mat4 transform;

#ifdef SKINNED
    mat4 BoneTransform = gBones[bonesID[0]] * bonesWeight[0];
    BoneTransform += gBones[bonesID[1]] * bonesWeight[1];
    BoneTransform += gBones[bonesID[2]] * bonesWeight[2];
    BoneTransform += gBones[bonesID[3]] * bonesWeight[3];

	transform = model * BoneTransform;
#else
	transform = model;
#endif

	world = vec3(transform * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(transform))) * aNormal;  
    TexCoord = aTexCoord;

#ifdef NORMALMAP
	computeTBN(mat3(transpose(inverse(transform))), aNormal, tangent);
#endif

#ifdef SHADOWMAP
#if DIRECTION_LIGHTS > 0
	for (int i=0; i<DIRECTION_LIGHTS; i++) {
		dirLight_world[i] = dirlights[i].matrix * vec4(world, 1.0);
	}
#endif
#endif

#ifdef TEXTURE_ARRAY
	layer = aLayer;
#endif

    gl_Position = projection * view * vec4(world, 1.0);
}