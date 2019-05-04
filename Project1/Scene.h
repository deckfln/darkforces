#pragma once
#include <list>
#include "Mesh.h"
#include "framework/AmbientLight.h"
#include "Camera.h"
#include "glEngine/glProgram.h"

class Scene
{
	std::list <Mesh *> meshes;
	AmbientLight *lights[10];
	int current_light;
	Camera *camera;

public:
	Scene();
	Scene &addCamera(Camera *camera);
	Scene &addMesh(Mesh *mesh);
	Scene &addLight(AmbientLight *light);
	void draw(void);
	~Scene();
};
