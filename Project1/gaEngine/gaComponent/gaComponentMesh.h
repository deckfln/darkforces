#pragma once

#include "../gaComponent.h"
#include "../../framework/fwMesh.h"

class gaEntity;

namespace GameEngine
{
	class ComponentMesh : public gaComponent
	{
		fwMesh* m_mesh = nullptr;

		void onMove(gaMessage* message);						// move a mesh
		void onRotate(gaMessage* message);						// rotate a mesh
		void onWorldInsert(gaMessage* message);					// when the object is first injected in the world
		void onWorldRemove(gaMessage* message);					// when the object is removed from the world

	public:
		ComponentMesh();
		ComponentMesh(fwMesh *mesh);

		// getter/setter
		inline fwMesh* mesh(void) { return m_mesh; };

		// proxy to access the fwMesh function
		inline const fwAABBox& modelAABB(void) { return m_mesh->modelAABB(); };
		inline const glm::vec3& position(void) { return m_mesh->position(); };
		inline fwGeometry* get_geometry(void) { return m_mesh->get_geometry(); };
		inline void visibility(bool v) { m_mesh->set_visible(v); };
		void clone(fwMesh* source);
		void set(fwGeometry* geometry, fwMaterial* material);
		inline void set_scale(float f) { m_mesh->set_scale(f); };
		inline void worldMatrix(glm::mat4* m) { m_mesh->worldMatrix(m); };

		// debugger
		void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
		void debugGUIinline(void) override;					// display the component in the debugger

			// flight recorder status
		inline uint32_t recordSize(void);					// size of the component record
		uint32_t recordState(void* record);					// save the component state in a record
		uint32_t loadState(void* record);					// reload a component state from a record

		~ComponentMesh();
	};
};
