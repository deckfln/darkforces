#pragma once

#include <vector>
#include <map>
#include <glm/vec2.hpp>

#include "../../framework/fwHUDelement.h"
#include "../../framework/fwUniform.h"
#include "../../gaEngine/gaComponent.h"

class dfLevel;
class fwGeometry;
class glProgram;
class glVertexArray;
class fwMaterial;
class dfWall;

namespace DarkForces {
	namespace Component {
		class AutoMap : public gaComponent, public fwHUDelement
		{
			struct Layer {
				uint32_t m_nbVertices;				// number of walls really stored in the buffer
				std::vector<glm::vec2> m_vertices;	// list of walls : 2 vertices = 1 wall
				std::vector<int32_t> m_walls;		// list of colors index for walls : 2 vertices = 1 wall
			};
			dfLevel* m_level = nullptr;
			std::map<uint32_t, Layer> m_verticesPerLayer;
			std::map<uint32_t, size_t> m_wallsIndex;// offset of the wall in the aWall attribute

			fwUniform m_uniPosition;
			fwUniform m_uniRatio;
			glm::vec2 m_playerPosition;				// position of the center to act as center of the map
			float m_ratio = 1.0f;					// screen ratio

#ifdef _DEBUG
			fwMaterial* m_material = nullptr;
			glProgram* m_program = nullptr;
			fwGeometry* m_geometry = nullptr;
			glVertexArray* m_vertexArray = nullptr;

#endif
			void onEnterSector(gaMessage* message);	// display a sector when the player enters
			void onScreenResize(gaMessage* message);// set the new screen ratio

		public:
			AutoMap(void);
			AutoMap(dfLevel* level);
			void set(dfLevel* level);
			void draw(fwFlatPanel* panel) override;	// dedicated draw function
			void dispatchMessage(gaMessage* message) override;
		};
	}
}
