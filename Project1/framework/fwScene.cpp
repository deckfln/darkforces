#include "fwScene.h"
#include <iterator> 
#include <map>
#include <list>
#include <string>

#include "../glad/glad.h"

#include "materials/NormalHelperMaterial.h"

static NormalHelperMaterial normalHelper;
static glProgram *normalHelper_program = nullptr;

fwScene::fwScene()
{
}

fwScene &fwScene::addLight(Light *light)
{
	addChild(light);

	lights[current_light++] = light;
	return *this;
}

fwScene &fwScene::setOutline(glm::vec4 *_color)
{
	outline_material = new OutlineMaterial(_color);
	return *this;
}

void fwScene::parseChildren(fwObject3D *root, std::map<std::string, std::map<int, std::list <fwMesh *>>> &meshesPerMaterial, std::string &codeLights, std::string &defines)
{
	Material *material;
	fwMesh *mesh;
	std::string code;
	int materialID;

	std::list <fwObject3D *> _children = root->get_children();

	for (auto child : _children) {
		parseChildren(child, meshesPerMaterial, codeLights, defines);

		// only display meshes
		if (!child->is_class(MESH)) {
			continue;
		}

		mesh = (fwMesh *)child;

		if (mesh->is_visible()) {
			material = mesh->get_material();
			code = material->hashCode() + codeLights;
			materialID = material->getID();

			meshesPerMaterial[code][materialID].push_front(mesh);
			materials[materialID] = material;

			// Create the shader program if it is not already there
			if (programs.count(code) == 0) {
				std::string vertex = material->get_vertexShader();
				std::string fragment = material->get_fragmentShader();
				std::string geometry = material->get_geometryShader();
				programs[code] = new glProgram(vertex, fragment, geometry, defines);
			}
		}
	}
}

void fwScene::draw(Camera *camera)
{
	if (outline_material != nullptr && outline_program == nullptr) {
		outline_program = new glProgram(outline_material->get_vertexShader(), outline_material->get_fragmentShader(), "", "");
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
	std::map<std::string, std::map<int, std::list <fwMesh *>>> meshesPerMaterial;
	Material *material;

	std::list <fwMesh *> ::iterator it;
	std::string code;
	int materialID;

	parseChildren(this, meshesPerMaterial, codeLights, defines);

	// draw all meshes per material
	std::map <int, std::list <fwMesh *>> listOfMaterials;
	std::list <fwMesh *> listOfMeshes;
	std::list <fwMesh *> listOfOutlinedMeshes;
	std::list <fwMesh *> normalHelperdMeshes;

	// setup camera
	camera->set_uniformBuffer();

	for (auto shader : meshesPerMaterial) {
		// draw all ojects sharing the same shader
		code = shader.first;
		listOfMaterials = shader.second;

		glProgram *program = programs[code];
		program->run();
		camera->bind_uniformBuffer(program);

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

				if (mesh->is_outlined()) {
					// write in the stencil buffer for outlined objects
					// record the object for later drawing
					glClear(GL_STENCIL_BUFFER_BIT);
					glEnable(GL_STENCIL_TEST);
					glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
					glStencilFunc(GL_ALWAYS, 1, 0xFF);

					glStencilMask(0xff);
				}

				/*
				 * main draw call
				 */
				mesh->draw(program);

				/*
				 * Outlined draw call
				 */
				if (mesh->is_outlined()) {
					// break execution flow
					glProgram *previous = program;

					outline_program->run();
					outline_material->set_uniforms(outline_program);

					camera->set_uniforms(outline_program);

					glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
					glStencilMask(0x00);

					mesh->draw(outline_program);

					glDisable(GL_STENCIL_TEST);

					// restore execution flow
					program->run();
				}

				/*
				 * normal helper draw call
				 */
				if (mesh->is_normalHelper()) {
					// break execution flow
					glProgram *previous = program;

					if (normalHelper_program == nullptr) {
						normalHelper_program = new glProgram(normalHelper.get_vertexShader(), normalHelper.get_fragmentShader(), normalHelper.get_geometryShader(), defines);
					}
					normalHelper_program->run();

					mesh->draw(normalHelper_program);

					// restore execution flow
					program->run();
				}
			}
		}
	}
}

fwScene::~fwScene()
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
