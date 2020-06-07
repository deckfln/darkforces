#version 330 core

layout (location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

uniform vec4 onscreen;

out vec2 TexCoords;

void main()
{
    TexCoords = aTex;

    vec4 p = vec4(aPos.x * onscreen.x, aPos.y * onscreen.y, 0.0, 1.0);
    p.x += onscreen.z;
    p.y += onscreen.w;

    gl_Position = p;
} 