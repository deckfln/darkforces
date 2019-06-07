 #version 330 core
      
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture;

void main()
{
	vec4 color = texture2D(texture, TexCoord);
    FragColor = color;
}