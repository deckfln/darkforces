#include "Scene.h"
#include <iterator> 


Scene::Scene()
{
}

Scene &Scene::addCamera(Camera *_camera)
{
	camera = _camera;
	return *this;
}

Scene &Scene::addMesh(Mesh *mesh)
{
	meshes.push_front(mesh);
	return *this;
}

Scene &Scene::addLight(Light *light)
{
	lights[current_light++] = light;
	return *this;
}

void Scene::draw(void)
{
	std::list <Mesh *> ::iterator it;
	for (it = meshes.begin(); it != meshes.end(); ++it) {
		glProgram &program = (*it)->run();

		// setup camera
		camera->set_uniforms(program);
		program.set_uniform("viewPos", camera->get_position());

		// setup lights
		for (int i=0; i<current_light; i++) {
			lights[i]->set_uniform(program);
		}

		(*it)->draw();
	}
}

Scene::~Scene()
{
}
