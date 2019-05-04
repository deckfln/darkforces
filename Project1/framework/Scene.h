#pragma once
#include <list>
#include "framework/Mesh.h"
#include "framework/AmbientLight.h"
#include "framework/Camera.h"

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
