#version 330 core
#define DEFINES

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;
layout (location = 6) in float aTextureID;
layout (location = 7) in float aAmbient;

#ifdef INSTANCED
layout (location = 4) in mat4 model;
#else
uniform mat4 model;
#endif

out vec3 ourColor;
out vec2 TexCoord;
out vec3 normal;
out vec3 world;
flat out uint textureID;	// index start in megatexture
flat out float ambient;

#ifdef NORMALMAP
	layout (location = 5) in vec3 tangent;
	out mat3 tbn;

	void computeTBN(mat3 normalMatrix, vec3 normal, vec3 tangent)
	{
		vec3 T = normalize(normalMatrix * tangent);
		vec3 N = normalize(normalMatrix * aNormal);
		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);
		tbn = mat3(T, B, N);
	}
#endif

#include "../../shaders/include/camera.glsl"

void main()
{
	world = vec3(model * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoord = aTexCoord;
	textureID = uint(aTextureID);
	ambient = aAmbient;

#ifdef NORMALMAP
	computeTBN(mat3(transpose(inverse(model))), aNormal, tangent);
#endif

    gl_Position = projection * view * vec4(world, 1.0);
}