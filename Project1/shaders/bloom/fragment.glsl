#version 330 core
out vec4 FragColor;

#define DEFINES

in vec2 TexCoord;

uniform sampler2D screenTexture;

uniform float horizontal;
uniform float vertical;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	vec2 texturepixels = textureSize(screenTexture, 0);
	vec2 pixelsize = 1.0/texturepixels;
	pixelsize = (1.0 - pixelsize) / texturepixels;

	vec2 hpixelsize = pixelsize/2.0;

	vec2 center = TexCoord.st;		// at the center of the pixel

	pixelsize.x *= horizontal;
	pixelsize.y *= vertical;

	vec4 orig = texture(screenTexture, TexCoord.st);
	float result = orig.a * weight[0];

	if (horizontal == 0) {
		for(int i = 1; i < 5; ++i)
		{
			result += texture(screenTexture, TexCoord + vec2(0, pixelsize.y * i)).a * weight[i];
			result += texture(screenTexture, TexCoord - vec2(0, pixelsize.y * i)).a * weight[i];
		}
	}
	else {
		for(int i = 1; i < 5; ++i)
		{
			result += texture(screenTexture, TexCoord + vec2(pixelsize.x * i, 0)).a * weight[i];
			result += texture(screenTexture, TexCoord - vec2(pixelsize.x * i, 0)).a * weight[i];
		}
	}

    FragColor = vec4(orig.rgb, result);
}  