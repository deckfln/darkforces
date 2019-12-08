#version 330 core
layout (location = 0) out vec3 gFragColor;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gWorld;
layout (location = 3) out vec3 gMaterial;

// uniform vec4 ourColor;
in vec3 ourColor;
in vec2 TexCoord;
in vec3 normal;
in vec3 world;

uniform sampler2D texture;

#include "../include/camera.glsl"

void main()
{
	vec4 color = texture2D(texture, gl_PointCoord.st);

    gFragColor = vec3(0, 0, 1);
	gWorld = world;
	gNormal = vec3(0);
	gMaterial = vec3(0);
}