#version 330 core
out vec4 FragColor;

#define DEFINES

in vec2 TexCoord;

uniform sampler2D screenTexture;
uniform vec2 pixelsize;

const float offset = 1.0 / 300.0;  

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

	float alpha = texture(screenTexture, TexCoord.st).a;

/*
	vec3 color;
	for (int i=0; i<3; i++) {
		for (int j=0; j<3; j++) {
			color += texture(screenTexture, TexCoord.st + vec2(pixelsize.x * i, pixelsize.y * j)).rgb;
		}
	}
	color /= 9;
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
*/

    vec3 color = texture(screenTexture, TexCoord.st).rgb;

#ifdef GAMMA_CORRECTION
    // apply gamma correction
    FragColor.rgb = pow(color, vec3(1.0/GAMMA_CORRECTION));
    FragColor.a = alpha;
#endif
}  