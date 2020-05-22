#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

struct SpriteModel {
	vec2 size;		// sprite size in pixel
	vec2 insert;	// offset of center point
	vec2 world;
	uvec2 statesIndex;	// pointer to the state table
};

in struct SpriteModel sm[];
flat in uint vTextureID[];	// index start in megatexture
flat out uint textureID;	// index start in megatexture

flat in float vAmbient[];	// index start in megatexture
flat out float ambient;	// index start in megatexture

out vec2 TexCoord;

void main()
{
    textureID = vTextureID[0]; // Point has only one vertex
    ambient = vAmbient[0]; // Point has only one vertex

	vec4 p1 = gl_in[0].gl_Position;

    p1.x += sm[0].insert.x;      // Point has only one vertex => [0]
    p1.y += sm[0].insert.y;
    gl_Position = p1;
	TexCoord = vec2(1.0, 1.0);
    EmitVertex();

    p1.y -= sm[0].size.y;
    gl_Position = p1;
	TexCoord = vec2(1.0, 0.0);	
	EmitVertex();

    p1.x += sm[0].size.x;
    p1.y += sm[0].size.y;
    gl_Position = p1;
	TexCoord = vec2(0.0, 1.0);
    EmitVertex();

    p1.y -= sm[0].size.y;
    gl_Position = p1;
	TexCoord = vec2(0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}