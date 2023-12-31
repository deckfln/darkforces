#pragma once

#include <list>
#include <glm/mat4x4.hpp>
#include "gaVolumeSpace.h"

class dfSuperSector;
class fwCamera;

namespace GameEngine {
	class Level {
	protected:
		std::list<dfSuperSector*> m_supersectors;
		VolumeSpace m_soundVolumes;								// voxel like volumes to propagate sounds
		glm::mat4 m_cachedCameraMatrix;							// to find if the camera changed between 2 frames
		dfSuperSector* m_lastSuperSector = nullptr;				// cached sector from the last findSector

	public:
		Level();
		void draw(dfSuperSector* sector, fwCamera* camera);		// draw recursively the sectors through the portals
		void hideSectors(void);									// hide all sectors and inform them
		dfSuperSector* findSector(const glm::vec3& position);	// return the sector the point is in
		inline VolumeSpace& volume(void) { return m_soundVolumes; };

		~Level();
	};
}

extern GameEngine::Level* g_gaLevel;
