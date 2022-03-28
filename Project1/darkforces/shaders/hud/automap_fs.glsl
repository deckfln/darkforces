#version 330 core
flat in int vColor;
out vec4 FragColor;
uniform int playerLayer;      // map layer

void main()
{
    int color = vColor & 0xff;
    int visible = (vColor >> 8) & 0xff;
    int layer = (vColor >> 16) & 0xff;

    FragColor = vec4(float(layer), 0, 0, 1);

    if (layer != playerLayer || visible != 0) {
        discard;                // only display the pixel if pn the same layer as the player
    }
    else {
        switch (color) {
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
}