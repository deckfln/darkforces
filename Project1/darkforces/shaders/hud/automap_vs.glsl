#version 330 core

layout (location = 0) in vec2 aPos; // wall vertices in GL space
layout (location = 2) in int aWall; // wall vertices in GL space

uniform vec2 player;    // player position in GL space

flat out int vColor;

void main()
{
//-29.06, -2.0, 24.75
    vec2 p1 = aPos / 10.0;
    vec4 p = vec4(-(p1.x - player.x) / 10.0, (p1.y - player.y) / 10.0, 0.0, 1.0);

    vColor = aWall;
    gl_Position = p;
} 