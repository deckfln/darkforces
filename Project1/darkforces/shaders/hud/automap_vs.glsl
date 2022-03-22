#version 330 core

layout (location = 0) in vec2 aPos; // wall vertices in GL space

uniform vec2 player;    // player position in GL space

void main()
{
//-29.06, -2.0, 24.75

    vec4 p = vec4((aPos.x - player.x) / 10.0, (aPos.y - player.y) / 10.0, 0.0, 1.0);
    gl_Position = p;
} 