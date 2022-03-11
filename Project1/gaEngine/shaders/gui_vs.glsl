#version 330 core

layout (location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

uniform vec4 positionsize;
uniform vec4 imagepos;
out vec2 TexCoords;

void main()
{
    switch (gl_VertexID) {
    case 0:
        TexCoords = vec2(imagepos.x,                imagepos.y + imagepos.w);
        break;
    case 1:
        TexCoords = vec2(imagepos.x,                imagepos.y);
        break;
    case 2:
        TexCoords = vec2(imagepos.x + imagepos.z,   imagepos.y);
        break;
    case 3:
        TexCoords = vec2(imagepos.x,                imagepos.y + imagepos.w);
        break;
    case 4:
        TexCoords = vec2(imagepos.x + imagepos.z,   imagepos.y);
        break;
    case 5:
        TexCoords = vec2(imagepos.x + imagepos.z,   imagepos.y + imagepos.w);
        break;
    }

    vec4 p = vec4(aPos.x * positionsize.z, aPos.y * positionsize.w, 0.0, 1.0);
    p.x += positionsize.x;
    p.y += positionsize.y;

    gl_Position = p;
} 