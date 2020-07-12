#pragma once

#include "../gaComponent.h"
#include "../../framework/fwMesh.h"

class fwGeometry;
class fwMaterial;
class gaEntity;

class gaComponentMesh : public gaComponent
{
	fwMesh m_mesh;
public:
	gaComponentMesh(void);
	gaComponentMesh(fwGeometry* _geometry, fwMaterial* _material);
	~gaComponentMesh();
};