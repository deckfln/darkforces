#include "gaVolumeSpace.h"

#include <map>
#include <queue>
#include <deque>
#include <vector>
#include <functional>
#include <math.h>       /* log10 */

#include <glm/glm.hpp>

static uint32_t g_ids = 0;

GameEngine::Sound::Volume::Volume(const fwAABBox& aabb):
	m_id(g_ids++),
	m_worldAABB(aabb)
{
}

/**
 *
 */
inline float GameEngine::Sound::Volume::vol(void)
{
	if (m_volume != 0) {
		return m_volume;
	}

	m_volume = m_worldAABB.volume();
	return m_volume;
}

GameEngine::VolumeSpace::VolumeSpace(void)
{
}

/**
 * create a new volume on the list
 */
uint32_t GameEngine::VolumeSpace::add(const fwAABBox& aabb)
{
	m_volumes.push_back(
		Sound::Volume(aabb)
	);

	return m_volumes.back().id();
}

/**
 * one way portal from id to id1
 */
void GameEngine::VolumeSpace::link(uint32_t id, uint32_t id1, const glm::vec3& center, float surface)
{
	m_volumes[id].linkTo(id1, center, surface);
}

/**
 * two-way portal from id to id1
 */
void GameEngine::VolumeSpace::link2(uint32_t id, uint32_t id1, const glm::vec3& center, float surface)
{
	m_volumes[id].linkTo(id1, center, surface);
	m_volumes[id1].linkTo(id, center, surface);
}

/**
 * find the smallest volume that has the point inside
 */
int32_t GameEngine::VolumeSpace::findVolume(const glm::vec3& p)
{
	float vMin = +INFINITY;
	float vl;
	int32_t volumeID = -1;

	for (auto& v : m_volumes) {
		if (v.isPointInside(p)) {
			vl = v.vol();
			if (vl < vMin) {
				volumeID = v.id();
				vMin = vl;
			}
		}
	}
	return volumeID;
}

/**
 * build all pathes between 2 points
 */
struct Node {
	uint32_t volumeID;
	glm::vec3 soundOrigin;

	inline Node(const uint32_t v, const glm::vec3& p) { volumeID = v; soundOrigin = p; };
};

struct Data {
	int32_t came_from;
	glm::vec3 came_from_portal;
	float cost_so_far;

	inline Data(const int32_t v, const float c, const glm::vec3& p) { 
		came_from = v; 
		cost_so_far = c; 
		came_from_portal = p; 
	};
	inline Data(void) { 
		came_from = -1; 
		cost_so_far = 0; 
		came_from_portal = glm::vec3(0); 
	};
};

void GameEngine::VolumeSpace::path(const glm::vec3& source, const glm::vec3& listener, float loundness, std::vector<GameEngine::Sound::Virtual>& xSolutions)
{
	// only do a volume navigation if the listener is likely to hear the sound
	float d = glm::distance(source, listener);
	float current_loundness = loundness - 20 * log10(d);
	if (current_loundness < 25.0f) {
		return;
	}

	int32_t vSource = findVolume(source);
	int32_t vListener = findVolume(listener);

	if (vSource < 0 || vListener < 0) {
		__debugbreak();
		return;
	}

	// if the 2 objects are in the same volume, stop there
	if (vSource == vListener) {
		xSolutions.push_back(Sound::Virtual(source, d));
		return;
	}

	// find all paths between the source and the listener unless the sound gets under 25db
	std::queue<Node> frontier;

	frontier.push(Node(vSource, source));

	std::map<uint32_t, Data> data;

	data[vSource] = Data(-1, 0, source);

	int32_t current = -1, next, prev = -1;
	float new_cost;
	glm::vec3 old_position;

	while (!frontier.empty()) {
		const Node& node = frontier.front();
		current = node.volumeID;
		old_position = node.soundOrigin;
		frontier.pop();

		uint32_t nbPortals = m_volumes[current].portal();
		for (int32_t i = 0; i < nbPortals; i++) {
			GameEngine::Sound::Portal& p = m_volumes[current].portal(i);
			next = p.volumeID();
			if (next < 0) {
				continue;
			}

			if (next == vListener) {
				std::vector<uint32_t> p;
				int32_t c = current;
				while (c > 0) {
					p.push_back(c);
					c = data[c].came_from;
				}
				float dx = glm::distance(listener, source);
				float d1 = glm::distance(listener, data[current].came_from_portal);
				float d2 = glm::distance(source, data[vSource].came_from_portal);
				float d = data[current].cost_so_far + d1 + d2;
				current_loundness = loundness - 20 * log10(d);

				if (current_loundness > 25.0f) {
					glm::vec3 direction = glm::normalize(data[current].came_from_portal - listener);
					glm::vec3 p = direction * d + listener;
					xSolutions.push_back(Sound::Virtual(p, d));
				}
				continue;
			}

			// only go through each volume once
			if (data.count(next) > 0) {
				continue;
			}

			// apply a sound attenuation based on the distance
			// under 40DB drop the path
			// https://www.wkcgroup.com/tools-room/inverse-square-law-sound-calculator/
			// https://www.earq.com/hearing-health/decibels

			new_cost = data[current].cost_so_far + glm::distance(old_position, p.center());
			current_loundness = loundness - 20 * log10(new_cost);

			// only continue the tree if the current sound is lound enough and the next portal is not blocked (transparency=0)
			if (current_loundness > 25.0f && m_volumes[next].transparency() > 0) {
				data[next] = Data(
					current,
					new_cost * m_volumes[next].transparency() * p.absorption(),
					p.center()
				);

				frontier.push(Node(next, p.center()));
			}
		}
	}
}

/**
 * set the volume transparency
 */
void GameEngine::VolumeSpace::transparency(uint32_t volumeID, float transparency)
{
	m_volumes[volumeID].transparency(transparency);
}
