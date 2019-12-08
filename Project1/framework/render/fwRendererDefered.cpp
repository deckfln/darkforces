#include "fwRendererDefered.h"

#include "../fwInstancedMesh.h"
#include "../mesh/fwMeshSkinned.h"
#include "../fwParticles.h"
#include "../postprocessing/fwPostProcessingDirectLight.h"
#include "../lights/fwDirectionLight.h"
#include "../materials/fwMaterialDepth.h"
#include "../materials/fwNormalHelperMaterial.h"
#include "../materials/fwMaterialDepth.h"
#include "../materials/fwBloomMaterial.h"
#include "../postprocessing/fwPostProcessingBloom.h"

static fwPostProcessingDirectLight *DirectionalLight;
static fwMaterialDepth materialDepth;

static glProgram* depth_program[3] = { nullptr, nullptr, nullptr };

fwRendererDefered::fwRendererDefered(int width, int height)
{
	// FRAME BUFFER
	m_colorMap = new glGBuffer(width * 2, height * 2);

	// bloom texture
	m_bloom = new fwPostProcessingBloom(width * 2, height * 2);

	DirectionalLight = new fwPostProcessingDirectLight(m_bloom->get_bloom_texture());
}

/**
 * pase all meshes, build
 *  list of opaque object, aranged by code > material > list of mshes
 *  list of transparent objects
 */
void fwRendererDefered::parseChildren(fwObject3D* root,
	std::map<std::string, std::map<int, std::list <fwMesh*>>>& opaqueMeshPerMaterial,
	std::list <fwMesh*>& transparentMeshes,
	std::map<std::string, std::map<int, std::list <fwMesh*>>>& particles,
	fwCamera* camera)
{
	fwMaterial* material;
	fwMesh* mesh;
	std::string code;
	int materialID;

	std::list <fwObject3D*> _children = root->get_children();

	for (auto child : _children) {
		parseChildren(child, opaqueMeshPerMaterial, transparentMeshes, particles, camera);

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

			if (mesh->is_transparent()) {
				transparentMeshes.push_front(mesh);
				mesh->extra(m_programs[code]);
			}
			else if (mesh->is_class(PARTICLES)) {
				particles[code][materialID].push_front(mesh);
				m_materials[materialID] = material;

			}
			else {
				opaqueMeshPerMaterial[code][materialID].push_front(mesh);
				m_materials[materialID] = material;
			}
		}
	}
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

/*
 *
 */
void fwRendererDefered::drawMeshes(std::map<std::string, std::map<int, std::list <fwMesh*>>> &meshesPerMaterial, fwCamera* camera)
{
	std::string code;
	int materialID;
	std::map <int, std::list <fwMesh*>> listOfMaterials;
	std::list <fwMesh*> listOfMeshes;
	fwMaterial* material;

	for (auto shader : meshesPerMaterial) {
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
				drawMesh(camera, mesh, program);
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
	std::map<std::string, std::map<int, std::list <fwMesh*>>> meshesPerMaterial;
	std::map<std::string, std::map<int, std::list <fwMesh*>>> particles;
	std::list <fwMesh*> transparentMeshes;
	fwMaterial* material;

	std::list <fwMesh*> ::iterator it;
	std::string code;
	int materialID;

	// build all shaders
	parseChildren(scene, meshesPerMaterial, transparentMeshes, particles, camera);

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
	drawMeshes(meshesPerMaterial, camera);

	// draw particles
	drawMeshes(particles, camera);

	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glDisable(GL_STENCIL_TEST);

	/*
	 * 3rd pass : directional lighting + generate bloom buffer
	 */
	// list all directional lights
	std::list <fwDirectionLight *> directionals;
	std::list <fwLight*> lights = scene->get_lights();

	for (auto light : lights) {
		if (light->getDefine() == "DIRECTION_LIGHTS") {
			directionals.push_front((fwDirectionLight *)light);
		}
	}

	glColorMap* outBuffer = nullptr;
	if (directionals.size() > 0) {
		outBuffer = DirectionalLight->draw((glGBuffer*)m_colorMap, directionals);
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
		// ignore the depth buffer test
		glRenderBuffer* previous = outBuffer->get_stencil();
		outBuffer->bindDepth(m_colorMap->get_stencil());

		background->draw(camera, GL_STENCIL_TEST);

		outBuffer->bindDepth(previous);
	}

	/*
	 * 4th pass: bloom pass
	 */
	if (m_bloom) {
		m_bloom->draw(outBuffer);
	}

	return outBuffer->getColorTexture(0);
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
}