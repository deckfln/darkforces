 #version 330 core
      
out vec4 FragColor;
in vec3 color;
uniform sampler2D texture;

void main()
{
	//vec4 color = texture2D(texture, gl_PointCoord.st);
    FragColor = vec4(color, 0.5);
}