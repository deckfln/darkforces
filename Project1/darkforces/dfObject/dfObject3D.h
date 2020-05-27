#pragma once

#include "../dfObject.h"

class df3DO;
class fwMesh;
class fwScene;
class dfVue;
class dfFileSystem;

class dfObject3D : public dfObject
{
	glm::vec3 m_animRotation = glm::vec3(0);
	glm::vec3 m_animRotationAxe = glm::vec3(0);
	float m_aniRotationSpeed = 0;	//rotates from -999 (max anti-clockwise) to 999 (max clockwise)

	dfVue* m_vue = nullptr;

	fwMesh* m_mesh = nullptr;		// object mesh
	bool m_pause = false;			// pause at the end of the animation

	time_t m_lastFrame = 0;			// timestamp of the last frame

public:
	dfObject3D(df3DO* threedo, glm::vec3& position, float ambient);
	void animRotationAxe(int axe);
	void animRotationSpeed(float s);
	void add2scene(fwScene* scene);
	void pause(bool p) { m_pause = p; };
	void vue(dfFileSystem *fs, std::string& vue, std::string& component);
	virtual bool update(time_t t);		// update based on timer
	~dfObject3D();
};