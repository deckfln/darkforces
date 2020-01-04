#include "fwRenderer.h"

#include "fwConstants.h"

#include "fwInstancedMesh.h"
#include "mesh/fwMeshSkinned.h"
#include "fwParticles.h"

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
		getAllChildren(child, meshes);

		// only display meshes
		if (!child->is_class(MESH)) {
			continue;
		}

		mesh = (fwMesh*)child;

		if (mesh->is_visible()) {
			if (mesh->is_class(SKINNED_MESH)) {
				meshes[FW_MESH_SKINNED].push_front(mesh);
			}
			else if (mesh->is_class(INSTANCED_MESH)) {
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
void fwRenderer::parseShaders(std::list <fwMesh *> &meshes, 
	std::string &defines, 
	std::string codeLights, 
	bool withShadow, 
	std::map<std::string, std::map<int, std::list <fwMesh*>>>& meshPerMaterial
	)
{
	std::string local_defines;
	fwMaterial* material;
	std::string code;
	GLuint materialID;

	for (auto mesh : meshes) {
		local_defines = defines;

		material = mesh->get_material();
		code = material->hashCode() + codeLights;
		materialID = material->getID();

		if (mesh->is_class(INSTANCED_MESH)) {
			local_defines += "#define INSTANCED\n";
			code += "INSTANCED";
		}

		if (mesh->is_class(SKINNED_MESH)) {
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
  */
void fwRenderer::parseChildren(fwObject3D* root, std::list <fwMesh *> meshes[], fwCamera* camera)
{
	fwMesh* mesh;

	std::list <fwObject3D*> _children = root->get_children();

	for (auto child : _children) {
		parseChildren(child, meshes, camera);

		// only display meshes
		if (!child->is_class(MESH)) {
			continue;
		}

		mesh = (fwMesh*)child;

		if (mesh->is_visible() && (mesh->always_draw() || camera->is_inFrustum(mesh))) {
			if (mesh->is_transparent()) {
				meshes[FW_RENDER_TRANSPARENT].push_front(mesh);
			}
			else if (mesh->is_class(PARTICLES)) {
				meshes[FW_RENDER_OPAQ_PARTICLES].push_front(mesh);
			}
			else {
				meshes[FW_RENDER_OPAQ].push_front(mesh);
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
	std::list <fwLight*> lights = scene->get_lights();
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
 * create a list of all lights o inject in the foreward rendering shader
 */
void fwRenderer::preProcessLights(fwScene *scene, std::map <std::string, std::list <fwLight*>> &lightsByType, std::string &defines, std::string &codeLights)
{
	// count number of lights
	std::list <fwLight*> lights = scene->get_lights();

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
	std::list <fwMesh *>& meshes,
	std::string& defines,
	std::string &codeLights,
	std::map <std::string, std::list <fwLight*>>& lightsByType,
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
	std::list <fwMesh*>& meshes,
	std::string& defines,
	std::string& codeLights,
	std::map <std::string, std::list <fwLight*>>& lightsByType,
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
void fwRenderer::sortMeshes(std::list<fwMesh *>& meshes, glm::vec3 cameraPosition) 
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