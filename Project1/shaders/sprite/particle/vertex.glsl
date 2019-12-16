#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

#include "../../include/camera.glsl"

out vec3 color;
out vec3 world;

uniform mat4 model;

// Constants (tweakable):
const float minPointScale = 0.1;
const float maxPointScale = 0.7;
const float maxDistance   = 100.0;

void main()
{
    // Calculate point scale based on distance from the viewer
    // to compensate for the fact that gl_PointSize is the point
    // size in rasterized points / pixels.
    float cameraDist = distance(aPos, viewPos);
    gl_PointSize = 200.0 / cameraDist;

	gl_Position = projection * view * vec4(aPos, 1.0);

   	world = vec3(model * vec4(aPos, 1.0));
    color = aColor;
}
