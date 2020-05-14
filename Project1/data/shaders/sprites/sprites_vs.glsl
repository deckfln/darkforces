#version 330 core
#define DEFINES

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aData;

uniform mat4 model;

struct SpriteModel {
    vec2 size; 
    vec2 insert;
    vec2 world;
    vec2 textureID;
};

layout (std140) uniform Models
{
    uniform SpriteModel modelTable[64];
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

	vTextureID = uint(aData.r);  // aTextureID)
	ambient = aData.g;          // aAmbient;
    uint modelID = uint(aData.g);
    sm = modelTable[modelID];
}