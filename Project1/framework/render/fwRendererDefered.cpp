#include <map>
#include <map>

#include "fwRendererDefered.h"

#include "../fwConstants.h"

#include "../../glEngine/glGBuffer.h"
#include "../fwCamera.h"
#include "../fwScene.h"
#include "../postprocessing/fwPostProcessingBloom.h"
#include "../fwInstancedMesh.h"
#include "../mesh/fwMeshSkinned.h"
#include "../fwParticles.h"
#include "../lights/fwDirectionLight.h"
#include "../materials/fwNormalHelperMaterial.h"
#include "../materials/fwBloomMaterial.h"
#include "../materials/fwMaterialDeferedLights.h"
#include "../postprocessing/fwPostProcessingBloom.h"
#include "../fwBackground.h"
#include "../fwHUD.h"

static glProgram* depth_program[3] = { nullptr, nullptr, nullptr };
static std::map<std::string, glProgram*> light_programs;

static float quadVertices[] = { -1.0f,  1.0f,	-1.0f, -1.0f,	 1.0f, -1.0f,	-1.0f,  1.0f,	 1.0f, -1.0f,	 1.0f,  1.0f };
static float quadUvs[] = { 	0.0f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,	0.0f, 1.0f,	1.0f, 0.0f,	1.0f, 1.0f };

static fwMaterialDeferedLight deferedLights;

fwRendererDefered::fwRendererDefered(int width, int height, bool withBloom)
{
	// FRAME BUFFER
	m_colorMap = new glGBuffer(width * 2, height * 2);

	// bloom texture
	if (withBloom) {
		m_bloom = new fwPostProcessingBloom(width * 2, height * 2);

		// final target for lightning
		m_lightRendering = new glColorMap(width * 2, height * 2, 1, 3, m_bloom->get_bloom_texture());	// 1 outgoing color buffer, no depthmap & no stencil map
	}
	else {
		m_lightRendering = new glColorMap(width * 2, height * 2, 1, 3, nullptr);	// 1 outgoing color buffer, no depthmap & no stencil map
	}

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
void fwRendererDefered::buildDeferedShader(const std::list <fwMesh*>& meshes,	
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
		materialID = material->id();

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
			const std::string& vertex = material->get_shader(VERTEX_SHADER);
			const std::string& geometry = material->get_shader(GEOMETRY_SHADER);
			const std::string& fragment = material->get_shader(FRAGMENT_SHADER, DEFERED_RENDER);
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
	// parse the m_lights in the scene to compute the code of the needed shader
	const std::list <fwLight*>& lights = scene->lights();

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
	std::map <std::string, std::string> variables;

	// add define for shadows
	if (shadowmap) {
		define += "#define SHADOWMAP\n";
	}

	// add defines for lightning
	if (directional_lights > 0) {
		define += "#define DIRECTION_LIGHTS " + std::to_string(directional_lights) + "\n";
	}
	if (point_lights) {
		define += "#define POINT_LIGHTS " + std::to_string(point_lights) + "\n";
	}
	if (m_bloom != nullptr) {
		define += "#define BLOOMMAP\n";
	}
	if (m_customLightning != "") {
		define += "#define CUSTOM_LIGHT\n";
		variables["CUSTOM_LIGHT_SHADER"] = m_customLightning;
	}

	// add the custom defines
	for (auto d : m_customDefines) {
		if (d.second) {
			define += "#define " + d.first;
		}
	}

	// Build the shader if it is missing
	if (light_programs[define] == nullptr) {
		std::string vertex = deferedLights.get_shader(VERTEX_SHADER);
		std::string fragment = deferedLights.get_shader(FRAGMENT_SHADER, FORWARD_RENDER, variables);

		// Bloom ?
		if (m_bloom != nullptr) {
			// setup the bloom source
			if (m_bloom != nullptr) {
				deferedLights.setBloomTexture(m_bloom->get_bloom_texture());
			}
		}

		// setup the GBuffer source
		deferedLights.setSourceTexture(m_colorMap->getColorTexture(0),
			m_colorMap->getColorTexture(1),
			m_colorMap->getColorTexture(2),
			m_colorMap->getColorTexture(3)
		);

		// shaders without shadow
		light_programs[define] = new glProgram(vertex, fragment, "", define);

		m_quad->bind();
		m_renderGeometry->enable_attributes(light_programs[define]);
		m_quad->unbind();
	}

	//TODO: add a set/restore for depth testing
	glDisable(GL_DEPTH_TEST);

	light_programs[define]->run();

	// setup m_lights
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
void fwRendererDefered::drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program, const std::string &defines)
{
	/*
		* main draw call
		*/
	mesh->draw(program);
}

/*
 *
 */
void fwRendererDefered::drawMeshes(const std::list <fwMesh*> &meshes, fwCamera* camera)
{
	std::string code;
	int materialID;
	std::map <int, std::list <fwMesh*>> listOfMaterials;
	std::list <fwMesh*> listOfMeshes;
	fwMaterial* material;
	std::string empty = "";
	std::map<std::string, std::map<int, std::list <fwMesh*>>> meshPerMaterial;

	buildDeferedShader(meshes, camera, meshPerMaterial);

	glm::vec3 cameraPosition = camera->get_position();

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

			fwRenderer::sortMeshes(listOfMeshes, cameraPosition);

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
	static const char* s1 = "drawshadow";
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(strlen(s1)), s1);
	bool hasShadowLights = drawShadows(camera, scene);
	glPopDebugGroup();

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
	static const char* s2 = "draw_opaque_objects";
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(strlen(s2)), s2);

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
	glPopDebugGroup();

	/*
	 * 4th pass : lighting + generate bloom buffer
	 * DO NOT overwrite the depth buffer with merging to the quad
	 */
	static const char* s3 = "merge_mtr";
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(strlen(s3)), s3);
	mergeMTR(scene);
	glPopDebugGroup();

	/*
	 * 5th pass : draw skybox
	 */
	static const char* s4 = "draw_skybox";
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(strlen(s4)), s4);

	m_lightRendering->bind();

	fwBackground* background = scene->background();
	if (background != nullptr) {
		// ignore the depth buffer test
		glRenderBuffer* previous = m_lightRendering->get_stencil();
		m_lightRendering->bindDepth(m_colorMap->get_stencil());

		background->draw(camera, GL_STENCIL_TEST);

		m_lightRendering->bindDepth(previous);
	}
	glPopDebugGroup();

	/*
	 * 6th pass: bloom pass
	 */
	if (m_bloom) {
		static const char* s5 = "draw_bloom";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(strlen(s5)), s5);

		m_bloom->draw(m_lightRendering);
		glPopDebugGroup();
	}


	/*
	 * 7th pass : draw transparent objects with forward rendering
	 */
	std::map <std::string, std::list <fwLight*>> lightsByType;
	std::string defines;
	std::string codeLights = "";

	static const char* s6 = "draw_transparent_objects";
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(strlen(s6)), s6);

	preProcessLights(scene, lightsByType, defines, codeLights);
	drawTransparentMeshes(
		camera,
		meshesPerCategory[FW_RENDER_TRANSPARENT],
		defines,
		codeLights,
		lightsByType,
		hasShadowLights);

	glPopDebugGroup();

	/*
	 * 8th pass: draw the HUD
	 */
	if (scene->hud() != nullptr) {
		static const char* s7 = "draw_hud";
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(strlen(s7)), s7);
		scene->hud()->draw();
		glPopDebugGroup();
	}
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