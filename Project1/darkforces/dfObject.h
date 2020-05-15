#pragma once
#include <string>
#include <glm/vec3.hpp>

class dfModel;

class dfObject
{
protected:
	float m_x=0, m_y=0, m_z=0;	// position in level space
	int m_state = 0;			// state of the object for WAX, unused for others
	int m_frame = 0;			// current frame to display based on frameSpeed
	time_t m_lastFrame = 0;

	dfModel* m_source;
public:
	dfObject(dfModel *source, float x, float y, float z);
	bool named(std::string name);
	std::string& model(void);
	bool updateSprite(glm::vec3* position, glm::vec3* texture);
	bool update(time_t t);		// update based on timer
	~dfObject();
};