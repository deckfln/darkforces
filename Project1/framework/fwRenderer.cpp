#include "fwRenderer.h"

#include "fwInstancedMesh.h"

fwRenderer::fwRenderer()
{
}

void fwRenderer::getAllChildren(fwObject3D* root, std::list <fwMesh*>& meshes, std::list <fwMesh*>& instances)
{
	fwMesh* mesh;

	std::list <fwObject3D*> _children = root->get_children();

	for (auto child : _children) {
		getAllChildren(child, meshes, instances);

		// only display meshes
		if (!child->is_class(MESH)) {
			continue;
		}

		mesh = (fwMesh*)child;

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