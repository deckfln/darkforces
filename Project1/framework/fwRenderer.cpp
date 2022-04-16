#include "fwRenderer.h"

#include <vector>
#include <queue>
#include "fwConstants.h"

#include "../glEngine/glColorMap.h"
#include "../glEngine/glProgram.h"

#include "fwObject3D.h"
#include "fwMesh.h"
#include "fwScene.h"
#include "fwMaterial.h"
#include "fwInstancedMesh.h"
#include "mesh/fwMeshSkinned.h"
#include "fwParticles.h"
#include "fwLight.h"
#include "fwMesh2D.h"

fwRenderer::fwRenderer()
{
}

/*
 * Create a list of visible meshes out of the scene 
 */
void fwRenderer::getAllChildren(fwObject3D* root, std::vector<std::list <fwMesh*>>& meshes)
{
	fwMesh* mesh;

	std::list <fwObject3D*> _children = root->get_children();

	for (auto child : _children) {
		// only display meshes
		mesh = dynamic_cast<fwMesh*>(child);
		if (mesh == nullptr) {
			continue;
		}

		if (mesh->visible()) {
			// if the parent mesh is not visible, ignore the children
			getAllChildren(child, meshes);

			fwMeshSkinned* skinned = dynamic_cast<fwMeshSkinned*>(mesh);
			fwInstancedMesh* instanced = dynamic_cast<fwInstancedMesh*>(mesh);
			if (skinned != nullptr) {
				meshes[FW_MESH_SKINNED].push_front(mesh);
			}
			else if (instanced != nullptr) {
				meshes[FW_MESH_INSTANCED].push_front(mesh);
			}
			else {
				meshes[FW_MESH_NORMAL].push_front(mesh);
			}
		}
	}
}

/***
 * Prepare the shaders for the list of meshes
 */
void fwRenderer::parseShaders(const std::list <fwMesh *>&meshes, 
	const std::string& defines, 
	const std::string& codeLights, 
	bool withShadow, 
	std::map<std::string, std::map<int, std::list <fwMesh*>>>& meshPerMaterial
	)
{
	std::string local_defines;
	fwMaterial* material;
	std::string code;
	GLuint materialID;
	fwMeshSkinned* skinned;
	fwInstancedMesh* instanced;

	for (auto mesh : meshes) {
		local_defines = defines;

		material = mesh->get_material();
		code = material->hashCode() + codeLights;
		materialID = material->id();

		skinned = dynamic_cast<fwMeshSkinned*>(mesh);
		if (skinned != nullptr) {
			local_defines += "#define INSTANCED\n";
			code += "INSTANCED";
		}

		instanced = dynamic_cast<fwInstancedMesh*>(mesh);
		if (instanced != nullptr) {
			local_defines += "#define SKINNED\n";
			code += "SKINNED";
		}

		if (withShadow && mesh->receiveShadow()) {
			local_defines += "#define SHADOWMAP\n";
			code += "SHADOWMAP";
		}

		// Create the shader program if it is not already there
		if (m_programs.count(code) == 0) {
			local_defines += material->defines();
			const std::string& vertex = material->get_shader(VERTEX_SHADER);
			const std::string& fragment = material->get_shader(FRAGMENT_SHADER);
			const std::string& geometry = material->get_shader(GEOMETRY_SHADER);
			m_programs[code] = new glProgram(vertex, fragment, geometry, local_defines);
		}

		m_materials[materialID] = material;
		meshPerMaterial[code][materialID].push_front(mesh);
	}
}

/***
 * Create lists of frustum visible meshes per type of objects
  *  list of opaque, transparent opaque particles object
  *  Update fwGeometry if they are dirty
  */
void fwRenderer::parseChildren(fwObject3D* root, std::list <fwMesh *> meshes[], fwCamera* camera)
{
	fwMesh* mesh;
	fwSprites* sprites;
	fwParticles* particles;

	const std::list <fwObject3D*>& _children = root->get_children();

	for (auto child : _children) {
		// only display meshes
		mesh = dynamic_cast<fwMesh*>(child);
		if (mesh==nullptr) {
			continue;
		}

		if (mesh->visible() && (mesh->always_draw() || camera->is_inFrustum(mesh))) {
			// if the parent mesh is not visible, ignore the children
			parseChildren(child, meshes, camera);


			if (mesh->is_transparent()) {
				meshes[FW_RENDER_TRANSPARENT].push_front(mesh);
			}
			else {
				sprites = dynamic_cast<fwSprites*>(mesh);
				particles = dynamic_cast<fwParticles*>(mesh);
				if (particles || sprites) {
					meshes[FW_RENDER_OPAQ_PARTICLES].push_front(mesh);
				}
				else {
					meshes[FW_RENDER_OPAQ].push_front(mesh);
				}
			}
		}
	}
}

/*
 * Draw the scene from the POV of each light source
 */
bool fwRenderer::drawShadows(fwCamera* camera, fwScene* scene)
{
	/*
	 * 1st pass Draw shadows
	*/
	bool hasShadowLights = false;
	const std::list <fwLight*>& lights = scene->lights();
	for (auto light : lights) {
		if (((fwObject3D*)light)->castShadow()) {
			hasShadowLights = true;

			// draw in the m_light shadowmap from the POV of the m_light
			light->startShadowMap();

			// get all objects to draw
			std::vector<std::list <fwMesh*>> meshes;
			meshes.resize(3);
			getAllChildren(scene, meshes);

			// 1st pass: single meshes


			// draw neareast first
			meshes[FW_MESH_NORMAL].sort([camera](fwMesh* a, fwMesh* b) { return a->sqDistanceTo(camera) < b->sqDistanceTo(camera); });
			meshes[FW_MESH_SKINNED].sort([camera](fwMesh* a, fwMesh* b) { return a->sqDistanceTo(camera) < b->sqDistanceTo(camera); });

			for (auto i = 0; i <= FW_MESH_SKINNED; i++) {
				light->runShadowProgram(i);
				for (auto mesh : meshes[i]) {
					if (mesh->castShadow()) {
						mesh->draw(light->getShadowProgram(i));
					}
				}
			}

			light->stopShadowMap();
		}
	}

	return hasShadowLights;
}

/*
 * create a list of all m_lights o inject in the foreward rendering shader
 */
void fwRenderer::preProcessLights(fwScene *scene, std::map <std::string, std::list <fwLight*>> &lightsByType, std::string &defines, std::string &codeLights)
{
	// count number of m_lights
	const std::list <fwLight*>& lights = scene->lights();

	for (auto light : lights) {
		lightsByType[light->getDefine()].push_front(light);
	}

	// pre-processor
	for (auto type : lightsByType) {
		codeLights += type.first + ":" + std::to_string(type.second.size());
		defines += "#define " + type.first + " " + std::to_string(type.second.size()) + "\n";
	}
}

/***
 * Draw meshes
 */
void fwRenderer::drawMeshes(
	fwCamera *camera,
	const std::list <fwMesh *>& meshes,
	const std::string& defines,
	const std::string &codeLights,
	const std::map <std::string, std::list <fwLight*>>& lightsByType,
	bool hasShadowLights
)
{
	std::map<std::string, std::map<int, std::list <fwMesh*>>> meshesPerMaterial;
	std::map <int, std::list <fwMesh*>> listOfMaterials;
	std::list <fwMesh*> listOfMeshes;
	std::list <fwMesh*> listOfOutlinedMeshes;
	std::list <fwMesh*> normalHelperdMeshes;
	std::string code;
	int materialID;
	fwMaterial* material;

	// create a list of mesh PER shaders and material
	parseShaders(
		meshes,
		defines,
		codeLights,
		hasShadowLights,
		meshesPerMaterial
	);

	glm::vec3 cameraPosition = camera->get_position();

	for (auto shader : meshesPerMaterial) {
		// draw all ojects sharing the same shader
		code = shader.first;
		listOfMaterials = shader.second;

		glProgram* program = m_programs[code];

		program->run();
		camera->bind_uniformBuffer(program);

		// setup m_lights
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
			sortMeshes(listOfMeshes, cameraPosition);

			glTexture::PushTextureUnit();
			material = m_materials[materialID];

			material->set_uniforms(program);

			for (auto mesh : listOfMeshes) {
				drawMesh(camera, mesh, program, defines);
			}

			glTexture::PopTextureUnit();
		}
	}
}

/***
 * Draw all transparent objects in forward rendering
 */
void fwRenderer::drawTransparentMeshes(
	fwCamera* camera,
	const std::list <fwMesh*>& meshes,
	const std::string& defines,
	const std::string& codeLights,
	const std::map <std::string, std::list <fwLight*>>& lightsByType,
	bool withShadow
)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	drawMeshes(
		camera,
		meshes,
		defines,
		codeLights,
		lightsByType,
		withShadow
	);

	glDisable(GL_BLEND);
}

/**
 * Sort meshed by z-order && distance
 */
void fwRenderer::sortMeshes(std::list<fwMesh *>& meshes, const glm::vec3& cameraPosition) 
{
	// draw neareast first. use both z-order and distance to camera
	meshes.sort([cameraPosition](fwMesh* a, fwMesh* b) {
		// zOrder's have priorities
		if (a->zOrder() != 0 && b->zOrder() != 0) {
			return a->zOrder() < b->zOrder();
		}
		// if one object has zOrder and not the other, it has priority
		if (a->zOrder() != 0) {
			return(true);
		}
		if (b->zOrder() != 0) {
			return(false);
		}
		// if zOrder not defined, use distance**2
		return a->sqDistance2boundingSphere(cameraPosition) < b->sqDistance2boundingSphere(cameraPosition);
	});
}

//--------------------------------------------------------

using mesh_per_material = std::map<fwMaterial*, std::vector<Framework::Mesh2D*>>;

static std::map<uint32_t, std::map<uint32_t, glVertexArray*>> g_vaos;
static std::map<int32_t, mesh_per_material> g_layers;
static std::map<uint32_t, fwMaterial*> m_materials;
static std::map<fwMaterial*, glProgram*> g_programs;

/**
 *
 */
static glVertexArray* getVAO(fwGeometry* geometry, glProgram* program, const std::string& name)
{
	uint32_t geometryID = geometry->id();
	uint32_t programID = program->id();

	glVertexArray* vao = g_vaos[geometryID][programID];
	if (vao == nullptr) {
		vao = new glVertexArray();
#ifdef _DEBUG
		vao->label(name);
#endif
		geometry->enable_attributes(program);
		vao->unbind();
		g_vaos[geometryID][programID] = vao;
	}

	return vao;
}

/**
 * draw 2D interface
 */
void fwRenderer::draw2D(fwScene* scene)
{
	static uint32_t m_frame = 0;

	std::list<Framework::Mesh2D*>& meshes2D = scene->meshes2D();

	// build a list of all meshed and children
	std::vector<Framework::Mesh2D*> meshes;
	std::queue<Framework::Mesh2D*> queue;

	for (auto mesh : meshes2D) {
		if (mesh->visible()) {
			queue.push(mesh);
			mesh->updateWorld(nullptr);
		}
	}

	Framework::Mesh2D* mesh = nullptr;
	while (queue.size() > 0) {
		mesh = queue.front();
		meshes.push_back(mesh);
		queue.pop();

		for (auto child : mesh->children()) {
			if (child->visible()) {
				queue.push(child);
			}
		}
	}

	// update the global list of mesh per material
	g_layers.clear();

	for (auto mesh : meshes) {
		fwMaterial* material = mesh->material();

		// ignore container (mesh without material or geometry)
		if (material) {
			fwGeometry* geometry= mesh->geometry();
			if (geometry) {
				g_layers[mesh->zOrder()][material].push_back(mesh);
			}
		}
	}
	m_frame++;

	// remove missing meshes

	static const char* s7 = "draw_meshes2d";
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(strlen(s7)), s7);
	glDisable(GL_DEPTH_TEST);								// disable depth test so screen-space quad isn't discarded due to depth test.

	// render meshes per layers
	for (auto& layer : g_layers) {
		// render meshes per material
		for (auto& materialMeshes : layer.second) {
			fwMaterial* material = materialMeshes.first;

			// compile the program if it is missing
			glProgram* program = g_programs[materialMeshes.first];
			if (program == nullptr) {
				std::string vs = material->load_shader(FORWARD_RENDER, VERTEX_SHADER, "");
				std::string fs = material->load_shader(FORWARD_RENDER, FRAGMENT_SHADER, "");
				std::string gs = material->load_shader(FORWARD_RENDER, GEOMETRY_SHADER, "");

				program = new glProgram(vs, fs, gs, "");

				g_programs[materialMeshes.first] = program;
			}

			program->run();
			material->set_uniforms(program);

			// parse all meshes
			for (auto& mesh : materialMeshes.second) {
				mesh->set_uniforms(program);
				fwGeometry* geometry = mesh->geometry();

				// build the VAO if it is missing
				glVertexArray* vao = getVAO(geometry, program, mesh->name());
				mesh->draw(vao);
			}
		}
	}
	glEnable(GL_DEPTH_TEST);

	glPopDebugGroup();
}

//--------------------------------------------------------

glm::vec2 fwRenderer::size(void)
{
	return m_colorMap->size();
}

void fwRenderer::start(void)
{
	m_colorMap->bind();
	m_colorMap->clear();
}

void fwRenderer::stop(void)
{
	m_colorMap->unbind();
}

fwRenderer::~fwRenderer()
{
}