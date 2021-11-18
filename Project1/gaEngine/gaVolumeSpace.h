#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include "../framework/fwAABBox.h"

namespace GameEngine {
	namespace Sound {
		class Virtual {
		public:
			glm::vec3 origin;
			float distance;
			inline Virtual(const glm::vec3& p, const float d) { origin = p; distance = d; }
		};

		class Portal {
			uint32_t m_id;					// index of the next volume in the list
			glm::vec3 m_center;
		public:
			Portal(uint32_t id, const glm::vec3& center) {
				m_id = id;
				m_center = center;
			}
			inline uint32_t volumeID(void) { return m_id; };
			inline const glm::vec3 center(void) { return m_center; };
		};

		class Volume {
			uint32_t m_id;
			fwAABBox m_worldAABB;
			float m_transparency = 1.0f;			// 1 = block all, 0 = block none
			std::vector<Portal> m_portals;
			float m_volume = 0;

		public:
			Volume(const fwAABBox& aabb);
			inline float vol(void);	// volume of the volume
			inline bool isPointInside(const glm::vec3& p) { return m_worldAABB.inside(p); };

			// getter/setter
			inline uint32_t id(void) { return m_id; };
			inline void linkTo(uint32_t id, const glm::vec3& center) { m_portals.push_back(Portal(id, center)); };
			inline uint32_t portal(void) { return m_portals.size(); };
			inline Portal& portal(uint32_t i) { return m_portals[i]; };
			inline const glm::vec3 center(void) { return m_worldAABB.center(); };
			inline const float transparency(void) { return m_transparency; };
		};
	};

	class VolumeSpace {
		std::vector<GameEngine::Sound::Volume> m_volumes;

		int32_t findVolume(const glm::vec3& p);							// find the smallest volume that has the point inside
	public:
		VolumeSpace(void);
		uint32_t add(const fwAABBox& aabb);								// create a new volume on the list
		void link(uint32_t id, uint32_t id1, const glm::vec3& center);	// one way portal from id to id1
		void link2(uint32_t id, uint32_t id1, const glm::vec3& center);	// two way portal from id to id1
		void path(
			const glm::vec3& source, 
			const glm::vec3& listener, 
			float loudness,												// sound loundness (in Decibel)
			std::vector<GameEngine::Sound::Virtual>& xSolutions
			);															// build a path between the 2 volumes
	};
}