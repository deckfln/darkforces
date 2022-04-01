#pragma once

#include <list>
#include <string>
#include "fwMesh.h"

class fwUniform;

namespace Framework
{
	class Mesh2D {
		int m_id = 0;

		std::string m_name;
		bool m_always_draw = false;				// ignore frustum visibility and always draw
		bool m_transparent = false;
		int32_t m_zorder = 0;					// 0 => sort meshed by distance to the camera
												// 1... => the app is sorting the meshes. 1 is drawn first, then 2 ....
		glVertexArray* m_vao = nullptr;

	protected:
		bool m_visible = true;					// object is displayed in the scene
		GLenum m_rendering = GL_TRIANGLES;	// Rendering mode of the mesh

		fwGeometry* m_geometry = nullptr;
		fwMaterial* m_material = nullptr;

	public:
		Mesh2D();
		Mesh2D(const std::string& name);
		Mesh2D(fwGeometry* _geometry, fwMaterial* _material, GLenum render = GL_TRIANGLES);

		inline int id(void) { return m_id; };
		void set(fwGeometry* geometry, fwMaterial* material, GLenum render = GL_TRIANGLES);

		//getter/setter
		inline void visible(bool visible) { m_visible = visible; };
		inline bool visible(void) { return m_visible; };
		inline int32_t zOrder(void) { return m_zorder; };
		inline void zOrder(int z) { m_zorder = z; };
		inline fwGeometry* geometry(void) { return m_geometry; };
		inline fwMaterial* material(void) { return m_material; };

		virtual void draw(glVertexArray*);

		~Mesh2D();
	};
}