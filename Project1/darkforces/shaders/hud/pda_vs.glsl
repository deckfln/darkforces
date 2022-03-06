#version 330 core

layout (location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

uniform vec4 onscreen;

out vec2 TexCoords;

void main()
{
    TexCoords = aTex;

    vec2 p2 = aPos * onscreen.w;    // scale
    p2 += onscreen.xy;

    vec4 p = vec4(p2.x, p2.y, 0.0, 1.0);
    gl_Position = p;
} 