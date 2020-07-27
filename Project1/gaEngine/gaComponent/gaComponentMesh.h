#pragma once

#include "../gaComponent.h"
#include "../../framework/fwMesh.h"

class fwGeometry;
class fwMaterial;
class gaEntity;

namespace GameEngine
{
	class ComponentMesh : public gaComponent, public fwMesh
	{
	public:
		ComponentMesh(void);
		ComponentMesh(fwGeometry* _geometry, fwMaterial* _material);
	
		void dispatchMessage(gaMessage* message) override;				// let a component deal with a situation

		~ComponentMesh();
	};
};
