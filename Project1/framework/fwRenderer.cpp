#include "fwRenderer.h"

#include "fwInstancedMesh.h"
#include "mesh/fwMeshSkinned.h"

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