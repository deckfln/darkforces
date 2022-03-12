#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec4 positionsize;
uniform vec4 imagepos;
out vec2 TexCoords;

void main()
{
    vec4 p = vec4(0, 0, 0.0, 1.0);

    switch (gl_VertexID) {
    case 0:
        TexCoords = vec2(imagepos.x,                imagepos.y + imagepos.w);
        p.x = positionsize.x;                       p.y = positionsize.y;
        break;
    case 1:
        TexCoords = vec2(imagepos.x,                imagepos.y);
        p.x = positionsize.x;                       p.y = positionsize.y + positionsize.w;
        break;
    case 2:
        TexCoords = vec2(imagepos.x + imagepos.z,   imagepos.y);
        p.x = positionsize.x + positionsize.z;      p.y = positionsize.y + positionsize.w;
        break;
    case 3:
        TexCoords = vec2(imagepos.x,                imagepos.y + imagepos.w);
        p.x = positionsize.x;                       p.y = positionsize.y;
        break;
    case 4:
        TexCoords = vec2(imagepos.x + imagepos.z,   imagepos.y);
        p.x = positionsize.x + positionsize.z;      p.y = positionsize.y + positionsize.w;
        break;
    case 5:
        TexCoords = vec2(imagepos.x + imagepos.z,   imagepos.y + imagepos.w);
        p.x = positionsize.x + positionsize.z;      p.y = positionsize.y;
        break;
    }

    p.x = p.x * 2.0f - aPos.x;
    p.y = aPos.y - p.y * 2.0f;

    gl_Position = p;
} 