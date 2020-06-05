// Dark forces custom lightning

float ambient = material.b;
float z = material.g;

const vec3 W1 = vec3(0.2125, 0.7154, 0.0721);
float lumis = dot(color, W1);

// bright pixels are not affected by the ambient dim, likely because they are supposed to be light source
if (lumis < 0.7) {
#ifdef HEADLIGHT
	color = color * (ambient + (1.0 - z));
#else
	color = color * ambient;
#endif
}
