#include <map>
#include "fwRendererDefered.h"

#include "../fwConstants.h"

#include "../fwInstancedMesh.h"
#include "../mesh/fwMeshSkinned.h"
#include "../fwParticles.h"
#include "../postprocessing/fwPostProcessingDirectLight.h"
#include "../lights/fwDirectionLight.h"
#include "../materials/fwNormalHelperMaterial.h"
#include "../materials/fwBloomMaterial.h"
#include "../materials/fwMaterialDeferedLights.h"
#include "../postprocessing/fwPostProcessingBloom.h"

static glProgram* depth_program[3] = { nullptr, nullptr, nullptr };
static std::map<std::string, glProgram*> light_programs;

static float quadVertices[] = { -1.0f,  1.0f,	-1.0f, -1.0f,	 1.0f, -1.0f,	-1.0f,  1.0f,	 1.0f, -1.0f,	 1.0f,  1.0f };
static float quadUvs[] = { 	0.0f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,	0.0f, 1.0f,	1.0f, 0.0f,	1.0f, 1.0f };

static fwMaterialDeferedLight deferedLights;

fwRendererDefered::fwRendererDefered(int width, int height)
{
	// FRAME BUFFER
	m_colorMap = new glGBuffer(width * 2, height * 2);

	// bloom texture
	m_bloom = new fwPostProcessingBloom(width * 2, height * 2);

	// final target for lightning
	m_lightRendering = new glColorMap(width * 2, height * 2, 1, 3, m_bloom->get_bloom_texture());	// 1 outgoing color buffer, no depthmap & no stencil map
	m_renderGeometry = new fwGeometry();

	m_quad = new glVertexArray();
	m_renderGeometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
	m_renderGeometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);
	m_quad->unbind();
}

/**
 * pase all meshes, build
 *  list of opaque object, aranged by code > material > list of mshes
 *  list of transparent objects
 */
void fwRendererDefered::buildDeferedShader(std::list <fwMesh*>& meshes,	
	fwCamera* camera,
	std::map<std::string, std::map<int, std::list <fwMesh*>>>& meshPerMaterial
	)
{
	fwMaterial* material;
	std::string code;
	int materialID;
	std::string local_defines;

	for (auto mesh: meshes) {
		local_defines = "";

		material = mesh->get_material();
		code = material->hashCode();
		materialID = material->getID();

		if (mesh->is_class(INSTANCED_MESH)) {
			local_defines += "#define INSTANCED\n";
			code += "INSTANCED";
		}

		if (mesh->is_class(SKINNED_MESH)) {
			local_defines += "#define SKINNED\n";
			code += "SKINNED";
		}

		// Create the shader program if it is not already there
		if (m_programs.count(code) == 0) {
			const std::string vertex = material->get_shader(VERTEX_SHADER);
			const std::string geometry = material->get_shader(GEOMETRY_SHADER);
			const std::string fragment = material->get_shader(FRAGMENT_SHADER, DEFERED_RENDER);
			m_programs[code] = new glProgram(vertex, fragment, geometry, local_defines);
		}

		m_materials[materialID] = material;
		meshPerMaterial[code][materialID].push_front(mesh);
	}
}

/**
 * Merge Multiple Rendering Target
 */
void fwRendererDefered::mergeMTR(fwScene *scene)
{
	// parse the lights in the scene to compute the code of the needed shader
	std::list <fwLight*> lights = scene->get_lights();

	int shadowmap = 0;
	int directional_lights = 0;
	int point_lights = 0;

	for (auto light : lights) {
		if (light->is_class(FW_DIRECTIONAL_LIGHT)) {
			directional_lights++;
		}
		if (light->is_class(FW_POINT_LIGHT)) {
			point_lights++;
		}
		if (((fwObject3D *)light)->castShadow()) {
			shadowmap = 1;
		}
	}

	std::string define = "";
	if (shadowmap) {
		define += "#define SHADOWMAP\n";
	}
	if (directional_lights > 0) {
		define += "#define DIRECTION_LIGHTS " + std::to_string(directional_lights) + "\n";
	}
	if (point_lights) {
		define += "#define POINT_LIGHTS " + std::to_string(point_lights) + "\n";
	}

	// Build the shader if it is missing
	if (light_programs[define] == nullptr) {
		std::string vertex = deferedLights.get_shader(VERTEX_SHADER);
		std::string fragment = deferedLights.get_shader(FRAGMENT_SHADER);

		// Bloom ?
		if (m_bloom != nullptr) {
			define += "#define BLOOMMAP\n";

			// setup the bloom source
			if (m_bloom != nullptr) {
				deferedLights.setBloomTexture(m_bloom->get_bloom_texture());
			}

			// setup the GBuffer source
			deferedLights.setSourceTexture(m_colorMap->getColorTexture(0),
				m_colorMap->getColorTexture(1),
				m_colorMap->getColorTexture(2),
				m_colorMap->getColorTexture(3)
			);
		}

		// shaders without shadow
		light_programs[define] = new glProgram(vertex, fragment, "", define);

		m_quad->bind();
		m_renderGeometry->enable_attributes(light_programs[define]);
		m_quad->unbind();
	}

	//TODO: add a set/restore for depth testing
	glDisable(GL_DEPTH_TEST);

	light_programs[define]->run();

	// setup lights
	int i = 0;
	for (auto light : lights) {
		light->set_uniform(light_programs[define], i);
		i++;
	}

	deferedLights.set_uniforms(light_programs[define]);

	m_lightRendering->bind();
	m_lightRendering->clear();

	if (m_bloom != nullptr) {
		m_colorMap->bindColors(2);	// activate bloom buffer
	}

	m_renderGeometry->draw(GL_TRIANGLES, m_quad);

	if (m_bloom != nullptr) {
		m_lightRendering->bindColors(1);	// deactivate boom buffer
	}

	m_lightRendering->unbind();

	glEnable(GL_DEPTH_TEST);

	// copy depth buffer from the draw buffer into the target buffer
	m_lightRendering->copyFrom(m_colorMap, GL_DEPTH_BUFFER_BIT);
}

/**
 * Draw a single mesh by program, apply outlone and normalHelpder if needed
 */
void fwRendererDefered::drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program, std::string &defines)
{
	/*
		* main draw call
		*/
	mesh->draw(program);
}

/*
 *
 */
void fwRendererDefered::drawMeshes(std::list <fwMesh*> &meshes, fwCamera* camera)
{
	std::string code;
	int materialID;
	std::map <int, std::list <fwMesh*>> listOfMaterials;
	std::list <fwMesh*> listOfMeshes;
	fwMaterial* material;
	std::string empty = "";
	std::map<std::string, std::map<int, std::list <fwMesh*>>> meshPerMaterial;

	buildDeferedShader(meshes, camera, meshPerMaterial);

	for (auto shader : meshPerMaterial) {
		// draw all ojects sharing the same shader
		code = shader.first;
		listOfMaterials = shader.second;

		glProgram* program = m_programs[code];

		program->run();
		camera->bind_uniformBuffer(program);

		for (auto ids : listOfMaterials) {
			// draw all ojects sharing the material
			materialID = ids.first;
			listOfMeshes = ids.second;

			// draw neareast first
			listOfMeshes.sort([camera](fwMesh* a, fwMesh* b) { return a->sqDistanceTo(camera) < b->sqDistanceTo(camera); });

			glTexture::PushTextureUnit();
			material = m_materials[materialID];
			material->set_uniforms(program);

			for (auto mesh : listOfMeshes) {
				drawMesh(camera, mesh, program, empty);
			}

			glTexture::PopTextureUnit();
		}
	}
}

/*
 *
 */
glTexture *fwRendererDefered::draw(fwCamera* camera, fwScene* scene)
{
	// update all elements on the m_scene
	scene->updateWorldMatrix(nullptr);

	/*
	 * 1st pass Draw shadows
	*/
	bool hasShadowLights = drawShadows(camera, scene);

	// create a map of materials shaders vs meshes
	//    [shaderCode][materialID] = [mesh1, mesh2]
	//std::map<std::string, std::map<int, std::list <fwMesh*>>> meshesPerMaterial;
	//std::map<std::string, std::map<int, std::list <fwMesh*>>> particles;
	//std::list <fwMesh*> transparentMeshes;

	std::list <fwMesh*> ::iterator it;
	std::string code;

	// sort objects by drawing order
	std::list <fwMesh*> meshesPerCategory[3];
	parseChildren(scene, meshesPerCategory, camera);
	//parseChildren1(scene, meshesPerMaterial, transparentMeshes, particles, camera);

	// draw all meshes per material
	std::map <int, std::list <fwMesh*>> listOfMaterials;
	std::list <fwMesh*> listOfMeshes;
	std::list <fwMesh*> listOfOutlinedMeshes;
	std::list <fwMesh*> normalHelperdMeshes;

	// setup m_camera
	camera->set_uniformBuffer();

	/*
	 * 2nd pass : draw opaque objects
	 */

	// record the stencil for the background draw
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 255, 0xFF);

	glStencilMask(0xff);
	glClear(GL_STENCIL_BUFFER_BIT);

	// draw opaque objects
	drawMeshes(meshesPerCategory[FW_RENDER_OPAQ], camera);

	/*
	 * 3rd pass : draw opaque particles
	 */
	drawMeshes(meshesPerCategory[FW_RENDER_OPAQ_PARTICLES], camera);

	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glDisable(GL_STENCIL_TEST);

	/*
	 * 4th pass : lighting + generate bloom buffer
	 * DO NOT overwrite the depth buffer with merging to the quad
	 */
	mergeMTR(scene);

	/*
	 * 5th pass : draw skybox
	 */
	m_lightRendering->bind();

	fwSkybox* background = scene->background();
	if (background != nullptr) {
		// ignore the depth buffer test
		glRenderBuffer* previous = m_lightRendering->get_stencil();
		m_lightRendering->bindDepth(m_colorMap->get_stencil());

		background->draw(camera, GL_STENCIL_TEST);

		m_lightRendering->bindDepth(previous);
	}

	/*
	 * 6th pass: bloom pass
	 */
	if (m_bloom) {
		m_bloom->draw(m_lightRendering);
	}


	/*
	 * 7th pass : draw transparent objects with forward rendering
	 */
	std::map <std::string, std::list <fwLight*>> lightsByType;
	std::string defines;
	std::string codeLights = "";

	preProcessLights(scene, lightsByType, defines, codeLights);
	drawTransparentMeshes(
		camera,
		meshesPerCategory[FW_RENDER_TRANSPARENT],
		defines,
		codeLights,
		lightsByType,
		hasShadowLights);

	return m_lightRendering->getColorTexture(0);
}

glm::vec2 fwRendererDefered::size(void)
{
	return m_colorMap->size();
}

glTexture* fwRendererDefered::getColorTexture(void)
{
	return m_colorMap->getColorTexture(GBUFFER_COLOR);
}

fwRendererDefered::~fwRendererDefered()
{
	delete m_colorMap;
	delete m_bloom;
	delete m_lightRendering;
	delete m_renderGeometry;
}