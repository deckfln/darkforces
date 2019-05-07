#pragma once
#include <list>
#include "../glEngine/glProgram.h"
#include "framework/Mesh.h"
#include "framework/Light.h"
#include "framework/Camera.h"

class Scene
{
	std::map <std::string, glProgram *> programs;

	std::list <Mesh *> meshes;
	Light *lights[10];
	int current_light;
	Camera *camera;

public:
	Scene();
	Scene &addCamera(Camera *camera);
	Scene &addMesh(Mesh *mesh);
	Scene &addLight(Light *light);
	void draw(void);
	~Scene();
};
