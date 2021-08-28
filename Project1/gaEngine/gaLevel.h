#pragma once

#include <list>
#include <glm/vec3.hpp>

class dfSuperSector;
class fwCamera;

namespace GameEngine {
	class Level {
	protected:
		std::list<dfSuperSector*> m_supersectors;
		dfSuperSector* m_lastSuperSector = nullptr;				// cached sector from the last findSector

	public:
		Level();
		void draw(const glm::vec3& position, fwCamera* camera);	// draw recursively the sectors through the portals
		dfSuperSector* findSector(const glm::vec3& position);	// return the sector the point is in
		~Level();
	};
}