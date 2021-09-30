#pragma once

#include "../../gaEngine/gaComponent.h"
#include "../../framework/fwAABBox.h"

class dfSector;
class dfWall;
class dfMesh;
class dfBitmap;

namespace DarkForces {
	namespace Component {
		class Sign : public gaComponent {
			uint32_t m_status = 0;
			dfBitmap* m_bitmap = nullptr;
			int m_start = 0;                    // index of the first attribute in the buffers
			int m_size = 0;                     // number of attributes

			fwAABBox m_localAABB;				// AABBox relative to the mesh the sign is applied on
			glm::vec3 m_position;

			dfMesh* m_mesh;						// mesh holding the geometry

			void buildGeometry(dfSector* sector, dfWall* wall, float z, float z1);
			void setStatus(int status);

		public:
			Sign(dfMesh* mesh, dfSector* sector, dfWall* wall, float z, float z1);
			void dispatchMessage(gaMessage* message) override;		// let the component deal with messages

			const fwAABBox& localAABB(void) { return m_localAABB; };
			const glm::vec3& position(void) { return m_position; };

			// debugger
			void debugGUIinline(void) override;						// display the component in the debugger
		};
	}
}