#version 330 core
#define DEFINES

layout (location = 0) in vec3 aPos;			// position of the object
layout (location = 1) in vec4 aData;		// state, frame ...
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

out vec3 vWorld;
out SpriteModel sm;

flat out uint vTextureID;	// index start in megatexture
flat out float vAmbient;
flat out float vAngle;

#include "../../../shaders/include/camera.glsl"

// how to pick the correct sprite based on the angle
uint spriteangle[32] = uint[]( 15, 14, 13, 12, 11, 10, 9 ,8 ,7, 6, 5, 4, 3, 2, 1, 0, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);

void main()
{
    // gl_Position = projection * view * vec4(aPos, 1.0);

    uint modelID = uint(aData.r);
    uint stateID = uint(aData.g);
    uint frameID = uint(aData.b);
	vAmbient = aData.a;

	if (modelID < uint(65536)) {
		sm.size = modelTable[modelID].size;
		sm.insert = modelTable[modelID].insert;
		sm.statesIndex = modelTable[modelID].statesIndex;
	
		if (sm.statesIndex.r < 65535) {
			// animated sprite
			vec3 viewer2object = normalize(aPos - viewPos);
			float viewAngle = (dot(aDirection, viewer2object) + 1) * 8.0;

			vec2 ad = vec2(aDirection.x, aDirection.z);
			vec2 vp = vec2(viewer2object.x, viewer2object.z);
			vAngle = dot(ad, vp);

			//https://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line
			//Use the sign of the determinant of vectors (AB,AM), where M(X,Y) is the query point:
			float det = sign(aDirection.x * viewer2object.z - aDirection.z * viewer2object.x);
			det = 1.0-clamp(det, 0, 1);

			uint angles = indexes[uint(stateID)+uint(sm.statesIndex.r)].x;
			uint frames = indexes[angles + spriteangle[uint(viewAngle + det * 16)]].y;
			vTextureID = indexes[frames + frameID].z;
		}
		else {
			vTextureID = uint(sm.statesIndex.g);
		}
	}
	else {
		// empty slow, draw nothing
		vTextureID = uint(65536);
	}

	vWorld = vec3(model * vec4(aPos, 1.0));

}