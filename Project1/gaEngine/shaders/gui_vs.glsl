#version 330 core

layout (location = 0) in vec2 aPos;

uniform vec4 imagepos;
uniform vec4 transformation;
out vec2 TexCoords;

void main()
{
    vec2 p = aPos * transformation.xy + transformation.wz;

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

    gl_Position = vec4(p, 0, 1);
} 