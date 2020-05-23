#version 330 core
layout (location = 0) out vec3 gFragColor;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gWorld;
layout (location = 3) out vec3 gMaterial;

// uniform vec4 ourColor;
in vec3 ourColor;
in vec3 world;
flat in float ambient;

#define DEFINES

void main()
{
    gFragColor = ourColor;
	gWorld = world;
	gNormal = vec3(0);

	// record approx depth for the headlight. the headlight cannot light over 5 units
	float z = gl_FragCoord.z / gl_FragCoord.w;
	gMaterial = vec3(0.0, z / 2.5f, 1.0);
}