#version 330 core
out vec4 FragColor;

#define DEFINES

in vec2 TexCoord;

uniform sampler2D screenTexture;

const float kernel[25] = float[](1.0, 4.0, 7.0, 4.0, 1.0,
								4.0, 16.0, 26.0, 16.0, 4.0,
								7.0, 26.0, 41.0, 26.0, 7.0,
								4.0, 16.0, 26.0, 16.0, 4.0,
								1.0, 4.0, 7.0, 4.0, 1.0);

void main()
{
	vec2 texturepixels = textureSize(screenTexture, 0);
	vec2 pixelsize = vec2(1.0)/texturepixels;
	// pixelsize = (vec2(1.0) - pixelsize) / texturepixels;

	vec2 hpixelsize = pixelsize/2.0;

	vec2 center = TexCoord.st;		// at the center of the pixel

	vec2 offst = center;
	vec4 pixel = texture(screenTexture, center);

	int total = 0;
	float alpha = 0;
	int p = 0;

	for(int i = -2; i < 3; i++)
	{
		for(int j = -2; j < 3; j++) {
			vec4 v = texture(screenTexture, center + vec2(i * pixelsize.x, j * pixelsize.y)) * kernel[p];

			pixel += v;
			p++;
		}
	}

	FragColor = vec4(pixel / 273.0);
}