#version 330 core
layout (location = 0) out vec3 gFragColor;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gWorld;
layout (location = 3) out vec3 gMaterial;

// uniform vec4 ourColor;
in vec3 ourColor;
in vec3 world;
in vec2 uv;
flat in int textureID;

uniform float ambient;
uniform sampler2D texture;
uniform vec4 megatexture_idx[512];

#define DEFINES

void main()
{
	vec4 color;

	if (textureID < 0) {
		color.rgb = ourColor;
	}
	else {
		vec2 vuv = fract(uv);

		// convert the local texture UV into the MegaTexture uv
		vec4 textureData = megatexture_idx[textureID];

		vec2 mega_uv = textureData.rg + textureData.ba * vuv;

		color = texture2D(texture, mega_uv); // color.rgba;
	}

    gFragColor = color.rgb;
	gWorld = world;
	gNormal = vec3(0);

	// record approx depth for the headlight. the headlight cannot light over 5 units
	float z = gl_FragCoord.z / gl_FragCoord.w;
	gMaterial = vec3(0.0, z / 2.5f, 1.0);
}