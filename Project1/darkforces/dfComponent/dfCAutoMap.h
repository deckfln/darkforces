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
			dfLevel* m_level = nullptr;
			std::map<uint32_t, size_t> m_wallsIndex;// offset of the wall in the aWall attribute

			std::vector<glm::vec2> m_vertices;	// list of walls : 2 vertices = 1 wall
			std::vector<int32_t> m_walls;		// list of colors index for walls : 2 vertices = 1 wall

			glm::vec2 m_playerPosition;				// position of the center to act as center of the map
			float m_ratio = 1.0f;					// screen ratio
			int32_t m_layer=1;						// layer of the player

			fwUniform m_uniPosition;
			fwUniform m_uniRatio;
			fwUniform m_uniLayer;

#ifdef _DEBUG
			fwMaterial* m_material = nullptr;
			glProgram* m_program = nullptr;
			fwGeometry* m_geometry = nullptr;
			glVertexArray* m_vertexArray = nullptr;

#endif
			void onEnterSector(gaMessage* message);	// display a sector when the player enters
			void onScreenResize(gaMessage* message);// set the new screen ratio
			void onShowAutomap(gaMessage* message);	// hide/display the automap

		public:
			AutoMap(void);
			AutoMap(dfLevel* level);
			void set(dfLevel* level);
			void draw(fwFlatPanel* panel) override;	// dedicated draw function
			void dispatchMessage(gaMessage* message) override;
		};
	}
}
