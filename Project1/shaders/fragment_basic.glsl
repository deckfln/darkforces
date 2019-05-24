#version 330 core
#define DEFINES

out vec4 FragColor;

uniform vec4 color;

#ifdef DIFFUSE_MAP
	in vec2 TexCoord;
    uniform sampler2D map;
#endif

void main()
{
#ifdef DIFFUSE_MAP
	vec4 diffuse = texture(map, TexCoord);
    FragColor  = diffuse;
#else
    FragColor  = color;
#endif
}