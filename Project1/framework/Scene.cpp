#include "Scene.h"
#include <iterator> 
#include <map>
#include <list>
#include <string>

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
	// count number of lights
	std::map <std::string, std::list <Light *>> lightsByType;

	for (int i = 0; i < current_light; i++) {
		lightsByType[ lights[i]->getDefine() ].push_front(lights[i]);
	}

	// pre-processor
	std::string defines;

	std::string codeLights="";
	for (auto type : lightsByType) {
		codeLights += type.first + ":" + std::to_string(type.second.size());
		defines += "#define " + type.first + " " + std::to_string(type.second.size()) + "\n";
	}

	// create a map of materials vs meshes
	std::map<std::string, std::list <Mesh *>> meshesPerMaterial;

	std::list <Mesh *> ::iterator it;
	std::string code;
	for (auto it : meshes) {
		code = it->getMaterialHash() + codeLights;

		meshesPerMaterial[code].push_front(it);

		// Create the program if it is not already there
		if (programs.count(code) == 0) {
			Material *material = it->get_material();

			programs[code] = new glProgram(material->get_vertexShader(), material->get_fragmentShader(), defines);;
		}
	}

	// draw all meshes per material
	std::map <std::string, std::list <Mesh *>> ::iterator hashes;
	std::list <Mesh *> listOfMeshes;

	for (auto hashes : meshesPerMaterial) {
		code = hashes.first;
		listOfMeshes = hashes.second;

		glProgram *program = programs[code];
		program->run();

		// setup camera
		camera->set_uniforms(program);
		program->set_uniform("viewPos", camera->get_position());

		// setup lights
		for (int i = 0; i < current_light; i++) {
			lights[i]->set_uniform(program, i);
		}

		for (auto mesh: listOfMeshes) {
			mesh->draw(program);
		}
	}
}

Scene::~Scene()
{
}
