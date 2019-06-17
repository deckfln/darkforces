#version 330 core
out vec4 FragColor;

#define DEFINES

in vec2 TexCoord;

uniform sampler2D screenTexture;

void main()
{
	vec2 texturepixels = textureSize(screenTexture, 0);
	vec2 pixelsize = 1.0/texturepixels;
	pixelsize = (1.0 - pixelsize) / texturepixels;

	vec2 hpixelsize = pixelsize/2.0;

	vec2 center = TexCoord.st + hpixelsize;		// at the center of the pixel

	vec4 result = texture(screenTexture, center);

    FragColor = result;
}  