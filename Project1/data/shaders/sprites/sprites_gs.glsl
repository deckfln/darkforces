#version 330

#include "../../../shaders/include/camera.glsl"

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

struct SpriteModel {
    vec2 size;		// sprite size in pixel
    vec2 insert;	// offset of center point
    vec2 world;
    uvec2 statesIndex;	// pointer to the state table
};

in struct SpriteModel sm[];
in vec3 world[];

flat in uint vTextureID[];	// index start in megatexture
flat out uint textureID;	// index start in megatexture

flat in float vAmbient[];	// index start in megatexture
flat out float ambient;	// index start in megatexture

out vec2 TexCoord;

void main()
{
    textureID = vTextureID[0]; // Point has only one vertex
    ambient = vAmbient[0]; // Point has only one vertex

    vec3 p1 = world[0];  // gl_in[0].gl_Position;

    // force the sprite to face the camera
    // trick is : extend the original point along the camera right vector

    // http://ogldev.atspace.co.uk/www/tutorial27/tutorial27.html
    vec3 toCamera = normalize(viewPos - p1);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(toCamera, up));

    p1 -= right * sm[0].size.x/2.0;
    gl_Position = projection * view * vec4(p1, 1.0);
    TexCoord = vec2(1.0, 0.0);
    EmitVertex();

    p1.y += sm[0].size.y;
    gl_Position = projection * view * vec4(p1, 1.0);
    TexCoord = vec2(1.0, 1.0);
    EmitVertex();

    p1.y -= sm[0].size.y;
    p1 += right * sm[0].size.x;
    gl_Position = projection * view * vec4(p1, 1.0);
    TexCoord = vec2(0.0, 0.0);
    EmitVertex();

    p1.y += sm[0].size.y;
    gl_Position = projection * view * vec4(p1, 1.0);
    TexCoord = vec2(0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}