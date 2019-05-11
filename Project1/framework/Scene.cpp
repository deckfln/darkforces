#include "Scene.h"
#include <iterator> 
#include <map>
#include <list>
#include <string>

#include "../glad/glad.h"

Scene::Scene()
{
}

Scene &Scene::addCamera(Camera *_camera)
{
	camera = _camera;
	return *this;
}

Scene &Scene::addLight(Light *light)
{
	addChild(light);

	lights[current_light++] = light;
	return *this;
}

Scene &Scene::setOutline(glm::vec4 *_color)
{
	outline_material = new OutlineMaterial(_color);
	return *this;
}

void Scene::parseChildren(Object3D *root, std::map<std::string, std::map<int, std::list <Mesh *>>> &meshesPerMaterial, std::string &codeLights, std::string &defines)
{
	Material *material;
	Mesh *mesh;
	std::string code;
	int materialID;

	std::list <Object3D *> _children = root->get_children();

	for (auto child : _children) {
		parseChildren(child, meshesPerMaterial, codeLights, defines);

		// only display meshes
		if (!child->is_class(MESH)) {
			continue;
		}

		mesh = (Mesh *)child;

		if (mesh->is_visible()) {
			material = mesh->get_material();
			code = material->hashCode() + codeLights;
			materialID = material->getID();

			meshesPerMaterial[code][materialID].push_front(mesh);
			materials[materialID] = material;

			// Create the shader program if it is not already there
			if (programs.count(code) == 0) {
				programs[code] = new glProgram(material->get_vertexShader(), material->get_fragmentShader(), defines);
			}
		}
	}
}

void Scene::draw(void)
{
	if (outline_material != nullptr && outline_program == nullptr) {
		outline_program = new glProgram(outline_material->get_vertexShader(), outline_material->get_fragmentShader(), "");
	}

	// update all elements on the scene
	updateWorldMatrix(nullptr);

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

	parseChildren(this, meshesPerMaterial, codeLights, defines);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	// draw all meshes per material
	std::map <int, std::list <Mesh *>> listOfMaterials;
	std::list <Mesh *> listOfMeshes;
	std::list <Mesh *> listOfOutlinedMeshes;

	for (auto shader : meshesPerMaterial) {
		// draw all ojects sharing the same shader
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
			// draw all ojects sharing the material
			materialID = ids.first;
			listOfMeshes = ids.second;

			glTexture::resetTextureUnit();
			material = materials[materialID];

			material->bindTextures();
			material->set_uniforms(program);

			for (auto mesh : listOfMeshes) {
				// draw all objects
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
	if (outline_material && listOfOutlinedMeshes.size() > 0) {
		outline_program->run();
		outline_material->set_uniforms(outline_program);

		camera->set_uniforms(outline_program);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		for (auto mesh : listOfOutlinedMeshes) {
			mesh->draw(outline_program);
		}
	}

	glDisable(GL_STENCIL_TEST);
}

Scene::~Scene()
{
	if (outline_program != nullptr) {
		delete outline_program;
	}

	if (outline_material != nullptr) {
		delete outline_material;
	}

	for (auto program : programs) {
		delete program.second;
	}
}
