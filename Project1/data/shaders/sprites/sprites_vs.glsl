#version 330 core
#define DEFINES

layout (location = 0) in vec3 aPos;
layout (location = 6) in float aTextureID;
layout (location = 7) in float aAmbient;

uniform mat4 model;

out vec3 ourColor;
out vec3 world;
flat out uint textureID;	// index start in megatexture
flat out float ambient;


#include "../../../shaders/include/camera.glsl"

void main()
{
    // Calculate point scale based on distance from the viewer
    // to compensate for the fact that gl_PointSize is the point
    // size in rasterized points / pixels.
    float cameraDist = distance(aPos, viewPos);
    gl_PointSize = 200.0 / cameraDist;

    gl_Position = projection * view * vec4(world, 1.0);

	world = vec3(model * vec4(aPos, 1.0));
	textureID = uint(aTextureID);
	ambient = aAmbient;
}