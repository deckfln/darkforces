#pragma once

#include "../gaComponent.h"
#include "../../framework/fwMesh.h"

class gaEntity;

namespace GameEngine
{
	class ComponentMesh : public gaComponent
	{
		fwMesh* m_mesh = nullptr;
	public:
		ComponentMesh();
		ComponentMesh(fwMesh *mesh);

		// proxy to access the fwMesh function
		inline const fwAABBox& modelAABB(void) { return m_mesh->modelAABB(); };
		inline const glm::vec3& position(void) { return m_mesh->position(); };
		inline fwGeometry* get_geometry(void) { return m_mesh->get_geometry(); };
		inline void visibility(bool v) { m_mesh->set_visible(v); };
		void clone(fwMesh* source);
		void set(fwGeometry* geometry, fwMaterial* material);
		inline void set_scale(float f) { m_mesh->set_scale(f); };
		inline void worldMatrix(glm::mat4* m) { m_mesh->worldMatrix(m); };

		void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
		void debugGUIinline(void) override;					// display the component in the debugger

		~ComponentMesh();
	};
};
