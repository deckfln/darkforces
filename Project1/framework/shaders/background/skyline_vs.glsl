#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;

out vec2 TexCoords;

#include "../../../shaders/include/camera.glsl"

void main()
{
    TexCoords = aTex;
	mat4 remove_translation = mat4(mat3(view));
    vec4 pos = projection * remove_translation * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
} 