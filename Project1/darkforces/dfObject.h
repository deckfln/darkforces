#pragma once
#include <string>
#include <glm/vec3.hpp>

class dfModel;

class dfObject
{
protected:
	float m_x=0, m_y=0, m_z=0;				// position in level space
	float m_pch = 0, m_yaw = 0, m_rol = 0;	// orientation
											// YAW = value in degrees where 0 is at the "top of the screen when you look at the map". The value increases clockwise
	int m_difficulty = 0;

	int m_state = 0;			// state of the object for WAX, unused for others
	int m_frame = 0;			// current frame to display based on frameSpeed
	time_t m_lastFrame = 0;

	dfModel* m_source;
public:
	dfObject(dfModel *source, float x, float y, float z);
	void set(float pch, float yaw, float rol, int difficulty);
	bool named(std::string name);
	std::string& model(void);
	bool updateSprite(glm::vec3* position, glm::vec3* texture);
	bool update(time_t t);		// update based on timer
	~dfObject();
};