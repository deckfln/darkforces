#include "Scene.h"
#include <iterator> 
#include <map>
#include <list>
#include <string>

#include "../glad/glad.h"
#include "materials/OutlineMaterial.h"

glm::vec4 yellow(255, 255, 0, 255);
OutlineMaterial _material(yellow);
glProgram *_outline_program = nullptr;

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
	if (_outline_program == nullptr) {
		_outline_program = new glProgram(_material.get_vertexShader(), _material.get_fragmentShader(), "");
	}

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

	// create a map of materials shaders vs meshes
	//    [shaderCode][materialID] = [mesh1, mesh2]
	std::map<std::string, std::map<int, std::list <Mesh *>>> meshesPerMaterial;
	Material *material;

	std::list <Mesh *> ::iterator it;
	std::string code;
	int materialID;

	for (auto it : meshes) {
		if (it->is_visible()) {
			material = it->get_material();
			code = material->hashCode() + codeLights;
			materialID = material->getID();

			meshesPerMaterial[code][materialID].push_front(it);
			materials[materialID] = material;

			// Create the shader program if it is not already there
			if (programs.count(code) == 0) {
				programs[code] = new glProgram(material->get_vertexShader(), material->get_fragmentShader(), defines);
			}
		}
	}

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	// draw all meshes per material
	std::map <int, std::list <Mesh *>> listOfMaterials;
	std::list <Mesh *> listOfMeshes;
	std::list <Mesh *> listOfOutlinedMeshes;

	for (auto shader : meshesPerMaterial) {
		code = shader.first;
		listOfMaterials = shader.second;

		glProgram *program = programs[code];
		program->run();

		// setup camera
		camera->set_uniforms(program);
		program->set_uniform("viewPos", camera->get_position());

		// setup lights
		int i;
		for (auto type : lightsByType) {
			i = 0;
			for (auto light : type.second) {
				light->set_uniform(program, i);

				i++;
			}
		}

		for (auto ids : listOfMaterials) {
			materialID = ids.first;
			listOfMeshes = ids.second;

			glTexture::resetTextureUnit();
			material = materials[materialID];

			material->bindTextures();
			material->set_uniforms(program);

			for (auto mesh : listOfMeshes) {
				if (mesh->is_outlined()) {
					// write in the stencil buffer for outlined objects
					// record the object for later drawing
					glStencilMask(0xff);
					listOfOutlinedMeshes.push_front(mesh);
				}
				else {
					glStencilMask(0x00);
				}

				mesh->draw(program);
			}
		}
	}

	// draw the ouline of the outlined object
	if (listOfOutlinedMeshes.size() > 0) {
		_outline_program->run();
		_material.set_uniforms(_outline_program);

		camera->set_uniforms(_outline_program);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		for (auto mesh : listOfOutlinedMeshes) {
			mesh->draw(_outline_program);
		}
	}

	glDisable(GL_STENCIL_TEST);
}

Scene::~Scene()
{
	for (auto program : programs) {
		delete program.second;
	}
}
