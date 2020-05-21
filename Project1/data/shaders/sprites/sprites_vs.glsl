#version 330 core
#define DEFINES

layout (location = 0) in vec3 aPos;			// position of the object
layout (location = 1) in vec3 aData;		// state, frame ...
layout (location = 2) in vec3 aDirection;	// direction the object is looking to

uniform mat4 model;

struct SpriteModel {
	vec2 size;		// sprite size in pixel
	vec2 insert;	// offset of center point
	vec2 world;
	ivec2 statesIndex;	// pointer to the state table
};

layout (std140) uniform Models
{
    SpriteModel modelTable[32];
	uvec4 indexes[2048];	// x = stateIndex
							// y = angleIndex
							// z = frameIndex
							// w = unused
	int x, y, z;
};

out vec3 ourColor;
out vec3 world;
out struct SpriteModel sm;

flat out uint vTextureID;	// index start in megatexture
flat out float ambient;

#include "../../../shaders/include/camera.glsl"

// how to pick the correct sprite based on the angle
uint spriteangle[32] = uint[]( 15, 14, 13, 12, 11, 10, 9 ,8 ,7, 6, 5, 4, 3, 2, 1, 0, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);

void main()
{

	world = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(world, 1.0);

    uint modelID = uint(aData.r);
    uint stateID = uint(aData.g);
    uint frameID = uint(aData.b);

    sm = modelTable[modelID];

	vec3 viewer2object = normalize(aPos - viewPos);
	float viewAngle = (dot(aDirection, viewer2object) + 1) * 8.0;
	//https://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line
	//Use the sign of the determinant of vectors (AB,AM), where M(X,Y) is the query point:
	float det = sign(aDirection.x * viewer2object.z - aDirection.z * viewer2object.x);
	det = clamp(det, 0, 1);

	uint angles = indexes[uint(stateID) + uint(sm.statesIndex.r)].x;
	uint frames = indexes[angles + spriteangle[uint(viewAngle + det * 16)] ].y;
	vTextureID = indexes[frames + frameID].z;
}