#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D skyline;
uniform vec2 repeat;

void main()
{    
    FragColor = texture(skyline, TexCoords*repeat);
}