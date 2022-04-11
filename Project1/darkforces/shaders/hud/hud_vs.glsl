#version 330 core

layout (location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

uniform vec4 transformation;
out vec2 TexCoords;

void main()
{
    TexCoords = aTex;

    vec2 p = aPos * transformation.xy + transformation.wz;

    gl_Position = glm::vec4(p, 0, 1);
} 