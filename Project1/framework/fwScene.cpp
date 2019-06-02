#include "fwScene.h"
#include <iterator> 
#include <map>
#include <list>
#include <string>

#include "../glad/glad.h"

#include "materials/fwNormalHelperMaterial.h"
#include "materials/fwDepthMaterial.h"
#include "fwInstancedMesh.h"

static fwNormalHelperMaterial normalHelper;
static fwDepthMaterial materialDepth;

static glProgram *normalHelper_program = nullptr;
static glProgram *outline_program = nullptr;
static glProgram *outline_instanced_program = nullptr;
static glProgram *depth_program = nullptr;
static glProgram *depth_instanced_program = nullptr;

fwScene::fwScene()
{
}

fwScene &fwScene::addLight(fwLight *light)
{
	addChild(light);

	lights[current_light++] = light;
	return *this;
}

fwScene &fwScene::setOutline(glm::vec4 *_color)
{
	outline_material = new fwOutlineMaterial(_color);
	return *this;
}

void fwScene::allChildren(fwObject3D *root, std::list <fwMesh *> &meshes, std::list <fwMesh *> &instances)
{
	fwMesh *mesh;

	std::list <fwObject3D *> _children = root->get_children();

	for (auto child : _children) {
		allChildren(child, meshes, instances);

		// only display meshes
		if (!child->is_class(MESH)) {
			continue;
		}

		mesh = (fwMesh *)child;

		if (mesh->is_visible()) {
			if (mesh->is_class(INSTANCED_MESH)) {
				instances.push_front(mesh);
			}
			else {
				meshes.push_front(mesh);
			}
		}
	}
}

/**
 * pase all meshes, build
 *  list of opaque object, aranged by code > material > list of mshes
 *  list of transparent objects
 */
void fwScene::parseChildren(fwObject3D *root, 
	std::map<std::string, std::map<int, std::list <fwMesh *>>> &opaqueMeshPerMaterial, 
	std::list <fwMesh *> &transparentMeshes,
	std::string &codeLights,
	std::string &defines, 
	bool withShadow,
	fwCamera *camera)
{
	fwMaterial *material;
	fwMesh *mesh;
	std::string code;
	int materialID;

	std::list <fwObject3D *> _children = root->get_children();

	for (auto child : _children) {
		parseChildren(child, opaqueMeshPerMaterial, transparentMeshes, codeLights, defines, withShadow, camera);

		// only display meshes
		if (!child->is_class(MESH)) {
			continue;
		}

		mesh = (fwMesh *)child;

		if (mesh->is_visible() && camera->is_inFrustum(mesh)) {
			std::string local_defines = defines;

			material = mesh->get_material();
			code = material->hashCode() + codeLights;
			materialID = material->getID();

			if (mesh->is_class(INSTANCED_MESH)) {
				local_defines += "#define INSTANCED\n";
				code += "INSTANCED";
			}

			if (withShadow && mesh->receiveShadow()) {
				local_defines += "#define SHADOWMAP\n";
				code += "SHADOWMAP";
			}

			// Create the shader program if it is not already there
			if (programs.count(code) == 0) {
				local_defines += material->defines();
				std::string vertex = material->get_vertexShader();
				std::string fragment = material->get_fragmentShader();
				std::string geometry = material->get_geometryShader();
				programs[code] = new glProgram(vertex, fragment, geometry, local_defines);
			}

			if (mesh->is_transparent()) {
				transparentMeshes.push_front(mesh);
				mesh->extra(programs[code]);
			}
			else {
				opaqueMeshPerMaterial[code][materialID].push_front(mesh);
				materials[materialID] = material;
			}
		}
	}
}

/**
 * Draw a single mesh by program, apply outlone and normalHelpder if needed
 */
void fwScene::drawMesh(fwCamera *camera, fwMesh *mesh, glProgram *program, std::string defines)
{
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
		glProgram *current = nullptr;

		if (mesh->is_class(INSTANCED_MESH)) {
			if (outline_instanced_program == nullptr) {
				outline_instanced_program = new glProgram(outline_material->get_vertexShader(), outline_material->get_fragmentShader(), "", "#define INSTANCED");
			}
			current = outline_instanced_program;
		}
		else {
			if (outline_program == nullptr) {
				outline_program = new glProgram(outline_material->get_vertexShader(), outline_material->get_fragmentShader(), "", "");
			}
			current = outline_program;
		}

		current->run();
		outline_material->set_uniforms(current);
		camera->bind_uniformBuffer(current);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		mesh->draw(current);

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
		camera->bind_uniformBuffer(normalHelper_program);

		mesh->draw(normalHelper_program);

		// restore execution flow
		program->run();
	}
}

void fwScene::draw(fwCamera *camera)
{
	if (outline_material != nullptr && outline_program == nullptr) {
		outline_program = new glProgram(outline_material->get_vertexShader(), outline_material->get_fragmentShader(), "", "");
	}

	// update all elements on the m_scene
	updateWorldMatrix(nullptr);

	/*
	 * 1st pass Draw shadows 
	 */
	bool hasShadowLights = false;
	for (int i = 0; i < current_light; i++) {
		fwLight *light = lights[i];
		if (((fwObject3D *)light)->castShadow()) {
			hasShadowLights = true;

			if (depth_program == nullptr) {
				depth_program = new glProgram(materialDepth.get_vertexShader(), materialDepth.get_fragmentShader(), "", "");
			}
			if (depth_instanced_program == nullptr) {
				depth_instanced_program = new glProgram(materialDepth.get_vertexShader(), materialDepth.get_fragmentShader(), "", "#define INSTANCED\n");
			}

			// draw in the m_light shadowmap from the POV of the m_light
			light->startShadowMap();
			light->setShadowCamera(depth_program);

			// get all objects to draw
			std::list <fwMesh *> meshes;
			std::list <fwMesh *> instances;
			allChildren(this, meshes, instances);

			// 1st pass: single meshes

			depth_program->run();

			// draw neareast first
			meshes.sort([camera](fwMesh *a, fwMesh *b) { return a->sqDistanceTo(camera) < b->sqDistanceTo(camera); });

			for (auto mesh : meshes) {
				if (mesh->castShadow()) {
					mesh->draw(depth_program);
				}
			}

			// 2nd pass: instanced meshes
			// TODO: sort instances by distance from the light
			depth_instanced_program->run();

			for (auto mesh : instances) {
				if (mesh->castShadow()) {
					mesh->draw(depth_instanced_program);
				}
			}

			light->stopShadowMap();
		}
	}

	// count number of lights
	std::map <std::string, std::list <fwLight *>> lightsByType;

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
	std::list <fwMesh *> transparentMeshes;
	fwMaterial *material;

	std::list <fwMesh *> ::iterator it;
	std::string code;
	int materialID;

	parseChildren(this, meshesPerMaterial, transparentMeshes, codeLights, defines, hasShadowLights, camera);

	// draw all meshes per material
	std::map <int, std::list <fwMesh *>> listOfMaterials;
	std::list <fwMesh *> listOfMeshes;
	std::list <fwMesh *> listOfOutlinedMeshes;
	std::list <fwMesh *> normalHelperdMeshes;

	// setup m_camera
	camera->set_uniformBuffer();

	/*
	 * 2nd pass : draw opaque objects
	 */
	for (auto shader : meshesPerMaterial) {
		// draw all ojects sharing the same shader
		code = shader.first;
		listOfMaterials = shader.second;

		glProgram *program = programs[code];
		glTexture::resetTextureUnit();

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

			// draw neareast first
			// TODO: sort instances by distance from the light
			listOfMeshes.sort([camera](fwMesh *a, fwMesh *b) { return a->sqDistanceTo(camera) < b->sqDistanceTo(camera); });

			glTexture::PushTextureUnit();
			material = materials[materialID];

			material->set_uniforms(program);

			for (auto mesh : listOfMeshes) {
				drawMesh(camera, mesh, program, defines);
			}

			glTexture::PopTextureUnit();
		}
	}

	/*
	 * 3rd pass : draw skybox
	 */
	if (m_pBackground != nullptr) {
		m_pBackground->draw(camera);
	}

	/*
	 * 4th pass : draw transparent objects
	 *	sorted from far to near
	 */
	transparentMeshes.sort([camera](fwMesh *a, fwMesh *b) { return a->sqDistanceTo(camera) > b->sqDistanceTo(camera); });

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (auto mesh: transparentMeshes) {
		material = mesh->get_material();

		glProgram *program = (glProgram *)mesh->extra();
		camera->bind_uniformBuffer(program);

		glTexture::resetTextureUnit();

		program->run();

		// setup lights
		int i;
		for (auto type : lightsByType) {
			i = 0;
			for (auto light : type.second) {
				light->set_uniform(program, i);

				i++;
			}
		}

		material->set_uniforms(program);
		drawMesh(camera, mesh, program, defines);
	}

	glDisable(GL_BLEND);

}

fwScene::~fwScene()
{
	for (auto program : programs) {
		delete program.second;
	}
}
