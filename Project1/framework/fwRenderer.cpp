#include "fwRenderer.h"

#include "fwInstancedMesh.h"
#include "mesh/fwMeshSkinned.h"
#include "materials/fwMaterialDepth.h"

static glProgram* depth_program[3] = { nullptr, nullptr, nullptr };
static fwMaterialDepth materialDepth;

fwRenderer::fwRenderer()
{
}

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
				meshes[SKINNED].push_front(mesh);
			}
			else if (mesh->is_class(INSTANCED_MESH)) {
				meshes[INSTANCED].push_front(mesh);
			}
			else {
				meshes[NORMAL].push_front(mesh);
			}
		}
	}
}

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

			if (depth_program[0] == nullptr) {
				depth_program[NORMAL] = new glProgram(materialDepth.get_shader(VERTEX_SHADER), materialDepth.get_shader(FRAGMENT_SHADER), "", "");
				depth_program[INSTANCED] = new glProgram(materialDepth.get_shader(VERTEX_SHADER), materialDepth.get_shader(FRAGMENT_SHADER), "", "#define INSTANCED\n");
				depth_program[SKINNED] = new glProgram(materialDepth.get_shader(VERTEX_SHADER), materialDepth.get_shader(FRAGMENT_SHADER), "", "#define SKINNED\n");
			}

			// draw in the m_light shadowmap from the POV of the m_light
			light->startShadowMap();
			light->setShadowCamera(depth_program[NORMAL]);

			// get all objects to draw
			std::vector<std::list <fwMesh*>> meshes;
			meshes.resize(3);
			getAllChildren(scene, meshes);

			// 1st pass: single meshes


			// draw neareast first
			meshes[NORMAL].sort([camera](fwMesh* a, fwMesh* b) { return a->sqDistanceTo(camera) < b->sqDistanceTo(camera); });
			meshes[SKINNED].sort([camera](fwMesh* a, fwMesh* b) { return a->sqDistanceTo(camera) < b->sqDistanceTo(camera); });

			for (auto i = 0; i <= SKINNED; i++) {
				depth_program[i]->run();
				for (auto mesh : meshes[i]) {
					if (mesh->castShadow()) {
						mesh->draw(depth_program[i]);
					}
				}
			}

			light->stopShadowMap();
		}
	}

	return hasShadowLights;
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