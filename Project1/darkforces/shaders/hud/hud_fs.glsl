#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform vec4 material;

void main()
{    
    FragColor = texture(image, TexCoords);
    if (FragColor.a == 0) {
        discard;
    }

	// Dark forces custom lightning

	float ambient = material.r;
	float headlight = material.g;
	float goggle = material.b;

	if (goggle != 0.0) {
		const vec3 W1 = vec3(0.2125, 0.7154, 0.0721);
		vec3 color = FragColor.rgb;
		float lumis = dot(color, W1);
		FragColor = vec4(0.0, lumis, 0.0, 1.0);
	}
	else if (headlight != 0.0) {
		
	}
	else {
		FragColor *= ambient;
	}
}