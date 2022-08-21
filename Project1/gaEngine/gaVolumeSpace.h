#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "../framework/fwAABBox.h"

#ifdef _DEBUG
#include "gaEntity.h"
#endif

namespace GameEngine {
	namespace Sound {
		/**
		 * position the sound is coming
		 */
		class Virtual {
		public:
			struct pointInSpace {
				glm::vec3 origin;
				float loundness;
			};
			uint32_t m_nbPoints;
			struct pointInSpace m_points[3];
			inline Virtual(void) {
				m_nbPoints = 0;
			}
		};

		class Portal {
			uint32_t m_id;					// index of the next volume in the list
			glm::vec3 m_center;
			float m_surface;
			float m_absortion;				// percentage of sound absorbtion based on surface
		public:
			Portal(uint32_t id, const glm::vec3& center, float surface):
				m_id(id),
				m_center(center),
				m_surface(surface)
			{		
				if (m_surface > 1.0f)
					m_absortion = 1.0f;
				else if (m_surface > 0.5f)
					m_absortion = 0.8f;
				else if (m_surface > 0.25f)
					m_absortion = 0.6f;
				else
					m_absortion = 0.4f;
			}
			inline uint32_t volumeID(void) { return m_id; };
			inline const glm::vec3 center(void) { return m_center; };
			inline const float absorption(void) { return m_absortion; };
		};

		class Volume {
			uint32_t m_id=0;
#ifdef _DEBUG
			gaEntity* m_entity = nullptr;
#endif
			fwAABBox m_worldAABB;
			float m_transparency = 1.0f;			// 1 = block all, 0 = block none
			std::vector<Portal> m_portals;			// list of portals to other volumes
			float m_volume = 0;						// volume of the space

		public:
			Volume(const fwAABBox& aabb, gaEntity* entity);
			inline float vol(void);	// volume of the volume
			inline bool isPointInside(const glm::vec3& p) { return m_worldAABB.inside(p); };

			// getter/setter
			inline uint32_t id(void) { return m_id; };
			inline void linkTo(uint32_t id, const glm::vec3& center, float surface) { m_portals.push_back(Portal(id, center, surface)); };
			inline uint32_t portal(void) { return m_portals.size(); };
			inline Portal& portal(uint32_t i) { return m_portals[i]; };
			inline const glm::vec3 center(void) { return m_worldAABB.center(); };
			inline const float transparency(void) { return m_transparency; };
			inline void transparency(const float transparency) { m_transparency = transparency; };
		};
	};

	class VolumeSpace {
		std::vector<GameEngine::Sound::Volume> m_volumes;

		int32_t findVolume(const glm::vec3& p);							// find the smallest volume that has the point inside
	public:
		VolumeSpace(void);
		uint32_t add(const fwAABBox& aabb, gaEntity* entity);							// create a new volume on the list
		void link(uint32_t id, uint32_t id1, const glm::vec3& center, float surface);	// one way portal from id to id1
		void link2(uint32_t id, uint32_t id1, const glm::vec3& center, float surface);	// two way portal from id to id1
		void path(
			const glm::vec3& source, 
			const glm::vec3& listener, 
			float loudness,												// sound loundness (in Decibel)
			std::vector<GameEngine::Sound::Virtual>& xSolutions
			);															// build a path between the 2 volumes
		void transparency(uint32_t volumeID, float transparency);		// set the volume transparency
	};
}