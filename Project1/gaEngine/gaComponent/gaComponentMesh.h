#pragma once

#include "../gaComponent.h"
#include "../../framework/fwMesh.h"

class fwGeometry;
class fwMaterial;
class gaEntity;

namespace GameEngine
{
	class ComponentMesh : public gaComponent
	{
		fwMesh m_mesh;

	public:
		ComponentMesh(void);
		ComponentMesh(fwGeometry* _geometry, fwMaterial* _material);
	
		void set_name(const std::string& name) { m_mesh.set_name(name); };
		void translate(glm::vec3& position) { m_mesh.translate(position); };
		void updateVertices() {m_mesh.updateVertices(); };				// reupload vertices to GPU
		void zOrder(int z) { m_mesh.zOrder(z); };
		const glm::mat4& worldMatrix(void) { return m_mesh.worldMatrix(); };
		const glm::mat4& inverseWorldMatrix(void) { return m_mesh.inverseWorldMatrix(); };
		void addChild(fwMesh* child) { m_mesh.addChild(child); };
		void updateWorldMatrix(ComponentMesh* m) {
			if (m != nullptr) {
				m_mesh.updateWorldMatrix(&m->m_mesh);
			}
			else {
				m_mesh.updateWorldMatrix(nullptr);
			}
		};
		void rotate(const glm::vec3& rotation) { m_mesh.rotate(rotation); };
		glm::vec3 get_position(void) {	return m_mesh.get_position(); };
		void set_visible(bool b) { m_mesh.set_visible(b); };
		fwMesh* mesh(void) { return &m_mesh; };

		void dispatchMessage(gaMessage* message) override;				// let a component deal with a situation

		~ComponentMesh();
	};
};
