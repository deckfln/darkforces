#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 TexCoord;

void main()
{
	vec4 aPos = gl_in[0].gl_Position;

    aPos.x += 0.5;
    aPos.y += 0.5;
    gl_Position = aPos;
	TexCoord = vec2(1.0, 0.0);
    EmitVertex();

    aPos.x -= 1.0;
    gl_Position = aPos;
	TexCoord = vec2(0.0, 0.0);	
	EmitVertex();

    aPos.x += 1.0;
    aPos.y -= 1.0;
    gl_Position = aPos;
	TexCoord = vec2(1.0, 1.0);
    EmitVertex();

    aPos.x -= 1.0;
    gl_Position = aPos;
	TexCoord = vec2(0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}