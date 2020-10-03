#pragma once

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include "../glad/glad.h"

#include "../gaEngine/gaModel.h"

// data for sprite model

struct Angle {
	unsigned short textureID[32];
};

struct State {
	struct Angle angles[32];
};

struct SpriteModel {
	glm::vec2 size;		// sprite size in pixel
	glm::vec2 insert;	// offset of center point
	glm::vec2 world;
	glm::ivec2 states;	// index of the start of the states
						// if x < 65535 : offset in the indexes table for state
						// if x === 6535
						//    y = textureID
};

struct GLmodel {
	SpriteModel models[32];
	glm::ivec4 indexes[2048]; // x = stateIndex
							// y = angleIndex
							// z = frameIndex
							// w = unused

	int stIndex = 0, atIndex = 0, ftIndex = 0;
};

#include "../framework/fwAABBox.h"

class fwMesh;

class dfModel: public GameEngine::gaModel
{
protected:
	glm::vec2 m_size_gl = glm::vec2(0);		// size in gl space (for sprites)
	glm::vec2 m_insert_gl = glm::vec2(0);	// offset in gl space from position (for sprites)
	bool m_testColision = false;			// can this object colide ?

	fwAABBox m_modelAABB;					// bounding box in model gl space
	fwMesh* m_meshAABB = nullptr;			// mesh showing the bounding box (GL model space)

	void updateBoundingBox(void);			// refresh the boundingbox based on m_size_gl & m_insert_gl

public:
	dfModel(const std::string& name, uint32_t myclass, bool collision = false);
	inline const fwAABBox& modelAABB(void) { return m_modelAABB; };
	inline bool collision(void) { return m_testColision; };
	inline const glm::vec2& sizeGL(void) { return m_size_gl; };
	fwMesh* drawBoundingBox(void);
	virtual int textureID(int state, int frame) { return 0; };
	virtual void spriteModel(GLmodel& model, int id);
	virtual int framerate(int state) { return 0; }
	virtual int nextFrame(int state, unsigned int frame) { return 0; };
};