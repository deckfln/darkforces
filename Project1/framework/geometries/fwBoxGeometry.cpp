#include "fwBoxGeometry.h"


float _vertices[] = {
	// back face
	-1.0f, -1.0f, -1.0f, // bottom-left
	 1.0f,  1.0f, -1.0f, // top-right
	 1.0f, -1.0f, -1.0f, // bottom-right         
	 1.0f,  1.0f, -1.0f, // top-right
	-1.0f, -1.0f, -1.0f, // bottom-left
	-1.0f,  1.0f, -1.0f, // top-left
	// front face
	-1.0f, -1.0f,  1.0f, // bottom-left
	 1.0f, -1.0f,  1.0f, // bottom-right
	 1.0f,  1.0f,  1.0f, // top-right
	 1.0f,  1.0f,  1.0f, // top-right
	-1.0f,  1.0f,  1.0f, // top-left
	-1.0f, -1.0f,  1.0f, // bottom-left
	// left face
	-1.0f,  1.0f,  1.0f, // top-right
	-1.0f,  1.0f, -1.0f, // top-left
	-1.0f, -1.0f, -1.0f, // bottom-left
	-1.0f, -1.0f, -1.0f, // bottom-left
	-1.0f, -1.0f,  1.0f, // bottom-right
	-1.0f,  1.0f,  1.0f, // top-right
	// right face
	 1.0f,  1.0f,  1.0f, // top-left
	 1.0f, -1.0f, -1.0f, // bottom-right
	 1.0f,  1.0f, -1.0f, // top-right         
	 1.0f, -1.0f, -1.0f, // bottom-right
	 1.0f,  1.0f,  1.0f, // top-left
	 1.0f, -1.0f,  1.0f, // bottom-left     
	// bottom face
	-1.0f, -1.0f, -1.0f, // top-right
	 1.0f, -1.0f, -1.0f, // top-left
	 1.0f, -1.0f,  1.0f, // bottom-left
	 1.0f, -1.0f,  1.0f, // bottom-left
	-1.0f, -1.0f,  1.0f, // bottom-right
	-1.0f, -1.0f, -1.0f, // top-right
	// top face
	-1.0f,  1.0f, -1.0f, // top-left
	 1.0f,  1.0f , 1.0f, // bottom-right
	 1.0f,  1.0f, -1.0f, // top-right     
	 1.0f,  1.0f,  1.0f, // bottom-right
	-1.0f,  1.0f, -1.0f, // top-left
	-1.0f,  1.0f,  1.0f // bottom-left        
};

float _uvs[] = {
	0.0f, 0.0f, // bottom-left
	1.0f, 1.0f, // top-right
	1.0f, 0.0f, // bottom-right         
	1.0f, 1.0f, // top-right
	0.0f, 0.0f, // bottom-left
	0.0f, 1.0f, // top-left
	// front face
	0.0f, 0.0f, // bottom-left
	1.0f, 0.0f, // bottom-right
	1.0f, 1.0f, // top-right
	1.0f, 1.0f, // top-right
	0.0f, 1.0f, // top-left
	0.0f, 0.0f, // bottom-left
	// left face
	1.0f, 0.0f, // top-right
	1.0f, 1.0f, // top-left
	0.0f, 1.0f, // bottom-left
	0.0f, 1.0f, // bottom-left
	0.0f, 0.0f, // bottom-right
	1.0f, 0.0f, // top-right
	// right face
	1.0f, 0.0f, // top-left
	0.0f, 1.0f, // bottom-right
	1.0f, 1.0f, // top-right         
	0.0f, 1.0f, // bottom-right
	1.0f, 0.0f, // top-left
	0.0f, 0.0f, // bottom-left     
	// bottom face
	0.0f, 1.0f, // top-right
	1.0f, 1.0f, // top-left
	1.0f, 0.0f, // bottom-left
	1.0f, 0.0f, // bottom-left
	0.0f, 0.0f, // bottom-right
	0.0f, 1.0f, // top-right
	// top face
	0.0f, 1.0f, // top-left
	1.0f, 0.0f, // bottom-right
	1.0f, 1.0f, // top-right     
	1.0f, 0.0f, // bottom-right
	0.0f, 1.0f, // top-left
	0.0f, 0.0f  // bottom-left        
};

float _normals[] = {
	// back face
	0.0f,  0.0f, -1.0f, // bottom-left
	0.0f,  0.0f, -1.0f, // top-right
	0.0f,  0.0f, -1.0f, // bottom-right         
	0.0f,  0.0f, -1.0f, // top-right
	0.0f,  0.0f, -1.0f, // bottom-left
	0.0f,  0.0f, -1.0f, // top-left
	// front face
	0.0f,  0.0f,  1.0f, // bottom-left
	0.0f,  0.0f,  1.0f, // bottom-right
	0.0f,  0.0f,  1.0f, // top-right
	0.0f,  0.0f,  1.0f, // top-right
	0.0f,  0.0f,  1.0f, // top-left
	0.0f,  0.0f,  1.0f, // bottom-left
	// left face
	-1.0f,  0.0f,  0.0f, // top-right
	-1.0f,  0.0f,  0.0f, // top-left
	-1.0f,  0.0f,  0.0f, // bottom-left
	-1.0f,  0.0f,  0.0f, // bottom-left
	-1.0f,  0.0f,  0.0f, // bottom-right
	-1.0f,  0.0f,  0.0f, // top-right
	// right face
	1.0f,  0.0f,  0.0f, // top-left
	1.0f,  0.0f,  0.0f, // bottom-right
	1.0f,  0.0f,  0.0f, // top-right         
	1.0f,  0.0f,  0.0f, // bottom-right
	1.0f,  0.0f,  0.0f, // top-left
	1.0f,  0.0f,  0.0f, // bottom-left     
	// bottom face
	0.0f, -1.0f,  0.0f, // top-right
	0.0f, -1.0f,  0.0f, // top-left
	0.0f, -1.0f,  0.0f, // bottom-left
	0.0f, -1.0f,  0.0f, // bottom-left
	0.0f, -1.0f,  0.0f, // bottom-right
	0.0f, -1.0f,  0.0f, // top-right
	// top face
	0.0f,  1.0f,  0.0f, // top-left
	0.0f,  1.0f,  0.0f, // bottom-right
	0.0f,  1.0f,  0.0f, // top-right     
	0.0f,  1.0f,  0.0f, // bottom-right
	0.0f,  1.0f,  0.0f, // top-left
	0.0f,  1.0f,  0.0f // bottom-left        
};


fwBoxGeometry::fwBoxGeometry()
{
	float *vertices = (float *)malloc(sizeof(_vertices));
	float *normals = (float *)malloc(sizeof(_normals));
	float *uvs = (float *)malloc(sizeof(_uvs));

	memcpy(vertices, _vertices, sizeof(_vertices));
	memcpy(normals, _normals, sizeof(_normals));
	memcpy(uvs, _uvs, sizeof(_uvs));

	addVertices("aPos", vertices, 3, sizeof(_vertices), ARRAY_SIZE_OF_ELEMENT(_vertices));
	addAttribute("aTexCoord", GL_ARRAY_BUFFER, uvs, 2, sizeof(_uvs), ARRAY_SIZE_OF_ELEMENT(_uvs));
	addAttribute("aNormal", GL_ARRAY_BUFFER, normals, 3, sizeof(_normals), ARRAY_SIZE_OF_ELEMENT(_normals));
}


fwBoxGeometry::~fwBoxGeometry()
{
}
