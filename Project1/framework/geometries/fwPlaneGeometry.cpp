#include "fwPlaneGeometry.h"
#include <math.h>


fwPlaneGeometry::fwPlaneGeometry(float width, float height, int widthSegments, int heightSegments)
{
	float width_half = width / 2;
	float height_half = height / 2;

	int gridX = floor(widthSegments);
	int gridY = floor(heightSegments);

	int gridX1 = gridX + 1;
	int gridY1 = gridY + 1;

	float segment_width = width / gridX;
	float segment_height = height / gridY;

	// buffers
	int len_indices = 6 * gridX * gridY;
	int *indices = (int *)calloc(len_indices, sizeof(int));

	int len_vertices = gridX1*gridX1;
	int len_uvs = gridX1*gridX1;

	glm::vec3 *vertices = (glm::vec3 *)calloc(len_vertices, sizeof(glm::vec3));
	glm::vec3 *normals = (glm::vec3 *)calloc(len_vertices, sizeof(glm::vec3));
	glm::vec2 *uvs = (glm::vec2 *)calloc(len_uvs, sizeof(glm::vec2));

	// generate vertices, normals and uvs
	int v = 0;
	int uv = 0;

	int ix, iy;

	for (iy = 0; iy < gridY1; iy++) {
		float y = iy * segment_height - height_half;

		for (ix = 0; ix < gridX1; ix++) {
			float x = ix * segment_width - width_half;

			vertices[v].x = x;
			vertices[v].y = -y;
			vertices[v].z = 0;

			normals[v].x = 0;
			normals[v].y = 0;
			normals[v].z = 1;

			v++;

			uvs[uv].x = ix / gridX;
			uvs[uv].y = 1 - (iy / gridY);

			uv++;
		}
	}

	// generate indices
	int a, b, c, d;
	v = 0;

	for (iy = 0; iy < gridY; iy++) {
		for (ix = 0; ix < gridX; ix++) {
			a = ix + gridX1 * iy;
			b = ix + gridX1 * (iy + 1);
			c = (ix + 1) + gridX1 * (iy + 1);
			d = (ix + 1) + gridX1 * iy;

			// faces
			indices[v] = a;
			indices[v + 1] = b;
			indices[v + 2] = d;

			indices[v + 3] = b;
			indices[v + 4] = c;
			indices[v + 5] = d;

			v += 6;
		}

	}

	len_vertices *= sizeof(glm::vec3);
	len_uvs *= sizeof(glm::vec2);
	len_indices *= sizeof(int);

	addVertices("aPos", vertices, 3, len_vertices, sizeof(float));

	addAttribute("aTexCoord", GL_ARRAY_BUFFER, uvs, 2, len_uvs, sizeof(float));
	addAttribute("aNormal", GL_ARRAY_BUFFER, normals, 3, len_vertices, sizeof(float));

	addIndex(indices, 1,len_indices, sizeof(unsigned int));
}


fwPlaneGeometry::~fwPlaneGeometry()
{
}
