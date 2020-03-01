float ambient = material.b;
float z = material.g;

#ifdef HEADLIGHT
color = color * (ambient + (1.0 - z));
#else
color = color * ambient;
#endif