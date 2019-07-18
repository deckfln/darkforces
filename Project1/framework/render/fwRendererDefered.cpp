#include "fwRendererDefered.h"

#include "../fwInstancedMesh.h"
#include "../postprocessing/fwPPDirectLight.h"
#include "../lights/fwDirectionLight.h"

static fwPPDirectLight DirectionalLight;

fwRendererDefered::fwRendererDefered(int width, int height)
{
	// FRAME BUFFER
	colorMap = new glGBuffer(width * 2, height * 2);
}

void fwRendererDefered::start(void)
{
	colorMap->bind();
	colorMap->clear();
}

void fwRendererDefered::stop(void)
{
	colorMap->unbind();
}

/**
 * Draw a single mesh by program, apply outlone and normalHelpder if needed
 */
void fwRendererDefered::drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program)
{
	/*
		* main draw call
		*/
	mesh->draw(program);
}

/**
 * pase all meshes, build
 *  list of opaque object, aranged by code > material > list of mshes
 *  list of transparent objects
 */
void fwRendererDefered::parseChildren(fwObject3D* root,
	std::map<std::string, std::map<int, std::list <fwMesh*>>>& opaqueMeshPerMaterial,
	std::list <fwMesh*>& transparentMeshes,
	fwCamera* camera)
{
	fwMaterial* material;
	fwMesh* mesh;
	std::string code;
	int materialID;

	std::list <fwObject3D*> _children = root->get_children();

	for (auto child : _children) {
		parseChildren(child, opaqueMeshPerMaterial, transparentMeshes, camera);

		// only display meshes
		if (!child->is_class(MESH)) {
			continue;
		}

		mesh = (fwMesh*)child;

		if (mesh->is_visible() && camera->is_inFrustum(mesh)) {
			std::string local_defines = "";

			material = mesh->get_material();
			code = material->hashCode();
			materialID = material->getID();

			if (mesh->is_class(INSTANCED_MESH)) {
				local_defines += "#define INSTANCED\n";
				code += "INSTANCED";
			}

			// Create the shader program if it is not already there
			if (programs.count(code) == 0) {
				local_defines += material->defines();
				const std::string vertex = material->get_shader(VERTEX_SHADER);
				const std::string geometry = material->get_shader(GEOMETRY_SHADER);
				const std::string fragment = material->get_shader(FRAGMENT_SHADER, DEFERED_RENDER);
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

glTexture *fwRendererDefered::draw(fwCamera* camera, fwScene* scene)
{
	// update all elements on the m_scene
	scene->updateWorldMatrix(nullptr);

	// create a map of materials shaders vs meshes
	//    [shaderCode][materialID] = [mesh1, mesh2]
	std::map<std::string, std::map<int, std::list <fwMesh*>>> meshesPerMaterial;
	std::list <fwMesh*> transparentMeshes;
	fwMaterial* material;

	std::list <fwMesh*> ::iterator it;
	std::string code;
	int materialID;

	// build all shaders
	parseChildren(scene, meshesPerMaterial, transparentMeshes, camera);

	// draw all meshes per material
	std::map <int, std::list <fwMesh*>> listOfMaterials;
	std::list <fwMesh*> listOfMeshes;
	std::list <fwMesh*> listOfOutlinedMeshes;
	std::list <fwMesh*> normalHelperdMeshes;

	// setup m_camera
	camera->set_uniformBuffer();

	/*
	 * 1nd pass : draw opaque objects
	 */
	for (auto shader : meshesPerMaterial) {
		// draw all ojects sharing the same shader
		code = shader.first;
		listOfMaterials = shader.second;

		glProgram* program = programs[code];
		glTexture::resetTextureUnit();

		program->run();
		camera->bind_uniformBuffer(program);

		for (auto ids : listOfMaterials) {
			// draw all ojects sharing the material
			materialID = ids.first;
			listOfMeshes = ids.second;

			// draw neareast first
			listOfMeshes.sort([camera](fwMesh* a, fwMesh* b) { return a->sqDistanceTo(camera) < b->sqDistanceTo(camera); });

			glTexture::PushTextureUnit();
			material = materials[materialID];

			material->set_uniforms(program);

			for (auto mesh : listOfMeshes) {
				drawMesh(camera, mesh, program);
			}

			glTexture::PopTextureUnit();
		}
	}

	/*
	 * 2nd pass : lighting
	 */
	// list all directional lights
	std::list <fwLight*> lights = scene->get_lights();
	std::list <fwDirectionLight *> directionals;

	for (auto light : lights) {
		if (light->getDefine() == "DIRECTION_LIGHTS") {
			directionals.push_front((fwDirectionLight *)light);
		}
	}

	glColorMap* outBuffer = nullptr;
	if (directionals.size() > 0) {
		outBuffer = DirectionalLight.draw(colorMap, directionals);
	}
	else {
		// FIXME
		// outBuffer = colorMap;
	}

	/*
	 * 3rd pass : draw skybox
	 */
	outBuffer->bind();

	fwSkybox* background = scene->background();
	if (background != nullptr) {
		background->draw(camera);
	}

	return outBuffer->getColorTexture(0);
}

glm::vec2 fwRendererDefered::size(void)
{
	return colorMap->size();
}

glTexture* fwRendererDefered::getColorTexture(void)
{
	return colorMap->getColorTexture();
}

fwRendererDefered::~fwRendererDefered()
{
	delete colorMap;
}