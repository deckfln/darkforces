#include "gaComponentMesh.h"

#include "../gaEntity.h"

gaComponentMesh::gaComponentMesh(void):
	gaComponent(GA_COMPONENT_MESH)
{
}

gaComponentMesh::gaComponentMesh(fwGeometry* _geometry, fwMaterial* _material):
	gaComponent(GA_COMPONENT_MESH),
	m_mesh(_geometry, _material)
{
}

gaComponentMesh::~gaComponentMesh()
{
}
