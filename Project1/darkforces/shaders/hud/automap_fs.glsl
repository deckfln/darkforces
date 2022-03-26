#version 330 core
flat in int vColor;
out vec4 FragColor;

void main()
{    
    switch (vColor) {
    case 1:
        FragColor  = vec4(0, 1, 0, 1);
        break;
    case 2:
        FragColor  = vec4(0, 0.4, 0, 1);
        break;
    case 3:
        FragColor  = vec4(1, 1, 0, 1);
        break;
    default:
        discard;
    }
}