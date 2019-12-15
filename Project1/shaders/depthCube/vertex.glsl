#version 330 core
layout (location = 0) in vec3 aPos;

#define DEFINES

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

    gl_Position = transform  * vec4(aPos, 1.0);
}