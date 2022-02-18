// Dark forces custom lightning

float ambient = material.b;
float z = material.g;

// TODO bright pixels are not affected by the ambient dim, likely because they are supposed to be light source
//if (lumis < 0.7) {

// detect case of non darkforces managed shader
if (material != vec3(0)) {
#ifdef HEADLIGHT
	#ifdef GREEN
		const vec3 W1 = vec3(0.2125, 0.7154, 0.0721);
		float lumis = dot(color, W1);
		color = vec3(0.0, lumis, 0.0);
	#else
		color = color * (ambient + (1.0 - z));
	#endif
#else
	#ifdef GREEN
		const vec3 W1 = vec3(0.2125, 0.7154, 0.0721);
		float lumis = dot(color, W1);
		color = vec3(0.0, lumis, 0.0);
	#else
		color = color * ambient;
	#endif
#endif
}

//}
