#version 330 core
out vec4 FragColor;

#define DEFINES

in vec2 TexCoord;

uniform sampler2D screenTexture;

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

	for(int i = -10; i < 11; i++)
	{
		for(int j = -10; j < 11; j++) {
			vec4 v = texture(screenTexture, center + vec2(i * pixelsize.x, j * pixelsize.y));

			if (v.rgb != vec3(0)) {
				pixel = max(pixel, v);
				alpha += pixel.a;
			}
			total++;
		}
	}

	FragColor = vec4(pixel.rgb, alpha/total * 2);
}  