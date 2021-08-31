#pragma once

#include <list>
#include <glm/mat4x4.hpp>

class dfSuperSector;
class fwCamera;

namespace GameEngine {
	class Level {
	protected:
		std::list<dfSuperSector*> m_supersectors;
		glm::mat4 m_cachedCameraMatrix;							// to find if the camera changed between 2 frames
		dfSuperSector* m_lastSuperSector = nullptr;				// cached sector from the last findSector

	public:
		Level();
		void draw(const glm::vec3& position, fwCamera* camera);			// draw recursively the sectors through the portals
		void hideSectors(void);											// hide all sectors and inform them
		dfSuperSector* findSector(const glm::vec3& position);			// return the sector the point is in
		~Level();
	};
}