#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 normal;
out vec3 world;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	world = vec3(model * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(world, 1.0);
}