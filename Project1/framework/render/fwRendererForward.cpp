#include "fwRendererForward.h"

#include "../fwConstants.h"

#include "../../glad/glad.h"

#include "../materials/fwNormalHelperMaterial.h"
#include "../materials/fwBloomMaterial.h"
#include "../fwInstancedMesh.h"
#include "../mesh/fwMeshSkinned.h"
#include "../fwSprites.h"
#include "../postprocessing/fwPostProcessingBloom.h"

static fwNormalHelperMaterial normalHelper;
fwOutlineMaterial* outline_material = nullptr;

static glProgram* normalHelper_program = nullptr;
static glProgram* outline_program = nullptr;
static glProgram* outline_instanced_program = nullptr;

static fwPostProcessingBloom *Bloom = nullptr;

void fwRendererForward::setOutline(glm::vec4* _color)
{
	outline_material = new fwOutlineMaterial(_color);
}

/**
 * Draw a single mesh by program, apply outlone and normalHelpder if needed
 */
void fwRendererForward::drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program, std::string &defines)
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
		glProgram* previous = program;
		glProgram* current = nullptr;

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
		glProgram* previous = program;

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

/*
 * Draw all meshes on the scane 
 */
glTexture *fwRendererForward::draw(fwCamera* camera, fwScene *scene)
{
	if (outline_material != nullptr && outline_program == nullptr) {
		outline_program = new glProgram(outline_material->get_vertexShader(), outline_material->get_fragmentShader(), "", "");
	}

	// update all elements on the m_scene
	scene->updateWorldMatrix(nullptr);

	/*
	 * 1st pass Draw shadows
	 */
	bool hasShadowLights = drawShadows(camera, scene);

	// count number of lights
	std::map <std::string, std::list <fwLight*>> lightsByType;
	std::string defines;
	std::string codeLights = "";

	preProcessLights(scene, lightsByType, defines, codeLights);

	// create a map of materials shaders vs meshes
	std::list <fwMesh*> meshesPerCategory[3];
	parseChildren(scene, meshesPerCategory, camera);

	// draw all meshes per material

	// setup m_camera
	camera->set_uniformBuffer();

	//m_colorMap->bind();
	//m_colorMap->clear();

	/*
	 * 2nd pass : draw opaque objects => FBO 0
	 *            record bright pixels => FBO 1
	 */
	drawMeshes(
		camera,
		meshesPerCategory[FW_RENDER_OPAQ],
		defines,
		codeLights,
		lightsByType,
		hasShadowLights
	);
	drawMeshes(
		camera,
		meshesPerCategory[FW_RENDER_OPAQ_PARTICLES],
		defines,
		codeLights,
		lightsByType,
		hasShadowLights
	);

	// remove the bloom buffer
	m_colorMap->bindColors(1);		// deactivate all color buffers but the first

	/*
	 * 4th pass : draw skybox
	 */
	fwSkybox* background = scene->background();
	if (background != nullptr) {
		background->draw(camera, GL_DEPTH_TEST);
	}

	/*
	 * 3rd pass : merge bloom buffer on the color buffer
	 */
	if (Bloom != nullptr) {
		Bloom->draw(m_colorMap);
	}

	 /*
	  * 5th pass : draw transparent objects
	  *	sorted from far to near
	  */
	drawTransparentMeshes(
		camera,
		meshesPerCategory[FW_RENDER_TRANSPARENT],
		defines,
		codeLights,
		lightsByType,
		hasShadowLights
	);

	m_colorMap->bindColors(2);		// activate all buffers

	return m_colorMap->getColorTexture(0);
}

fwRendererForward::fwRendererForward(int width, int height)
{
	// FRAME BUFFER
	Bloom = new fwPostProcessingBloom(width*2, height*2);

	m_colorMap = new glColorMap(width * 2, height * 2, 1, GL_COLORMAP_DEPTH | GL_COLORMAP_STENCIL,
		Bloom->get_bloom_texture()
	);
}

glTexture* fwRendererForward::getColorTexture(void)
{
	return m_colorMap->getColorTexture(0);
}

fwRendererForward::~fwRendererForward()
{
	delete m_colorMap;

	for (auto program : m_programs) {
		delete program.second;
	}
}
