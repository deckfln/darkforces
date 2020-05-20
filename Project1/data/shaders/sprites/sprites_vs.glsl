#version 330 core
#define DEFINES

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aData;

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
	uint stateTable[128];
	uint angleTable[2048];
	uint frameTable[512];

	int x, y, z;
};

out vec3 ourColor;
out vec3 world;
out struct SpriteModel sm;

flat out uint vTextureID;	// index start in megatexture
flat out float ambient;

#include "../../../shaders/include/camera.glsl"

void main()
{

	world = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(world, 1.0);

    uint modelID = uint(aData.r);
    uint stateID = uint(aData.g);
    uint frameID = uint(aData.b);

    sm = modelTable[modelID];

	uint angles = stateTable[uint(stateID) + uint(sm.statesIndex.r)];
	uint frames = angleTable[angles + uint(0)];
	vTextureID = frameTable[frames + frameID];
}