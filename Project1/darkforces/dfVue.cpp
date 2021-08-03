#include "dfVue.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

#include "dfFileSystem.h"
#include "dfParseINF.h"

dfVue::dfVue(dfFileSystem* fs, const std::string& file, const std::string& component)
{
	int size;
	char* sec = fs->load(DF_DARK_GOB, file, size);
	std::istringstream infile(sec);
	std::string line, dump;
	std::map<std::string, std::string> tokenMap;
	unsigned int frame = 0;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);
		if (tokens.size() == 0) {
			continue;
		}

		if (tokens[0] == "frame") {
			frame = std::stoi(tokens[1]);
		}
		else if (tokens[0] == "transform" && tokens[1] == component) {
			if (frame + 1 > m_animations.size()) {
				m_animations.resize(frame + 1);
			}
			glm::mat4x4 world = glm::mat4x4(
				std::stof(tokens[2]), std::stof(tokens[3]), std::stof(tokens[4]), 0.0f, 
				std::stof(tokens[5]), std::stof(tokens[6]), std::stof(tokens[7]), 0.0f,
				std::stof(tokens[8]), std::stof(tokens[9]), std::stof(tokens[10]), 0.0f,
				-std::stof(tokens[11]) , std::stof(tokens[13]) , std::stof(tokens[12]), 1.0f
			) / 10.0f;

			// decompose the matrix to get the rotation
			glm::vec3 scale;
			glm::quat rotation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(world, scale, rotation, translation, skew, perspective);
			rotation = glm::conjugate(rotation);

			// extract pitch, yaw, roll from rotation
			glm::vec3 pyr = glm::eulerAngles(rotation);

			m_animations[frame].m_position = translation;
			m_animations[frame].m_rotation = glm::vec3(pyr.x, 3.14159f - pyr.z, pyr.y);	// inverse pitch and yaw );
			m_animations[frame].m_scale = scale;
		}
	}

	delete sec;
}

/**
 * Return the worldMatrix of the first frame
 */
glm::mat4* dfVue::firstFrame(time_t t)
{
	static glm::mat4x4 matrice;

	m_currentFrame = 0;

	glm::vec3 translation = m_animations[0].m_position;
	glm::vec3 rotation = m_animations[0].m_rotation;
	glm::vec3 scale = m_animations[0].m_scale;

	glm::mat4 rMatrix = glm::eulerAngleXYX(rotation.x, rotation.y, rotation.z);	// inverse pitch and yaw 
	glm::mat4 sMatrix = glm::scale(scale / 10.0f);
	glm::mat4 tMatrix = glm::translate(translation / 10.0f);

	return &matrice;
}

/**
 * Return the interpolated worldMatrix of the frame
 */
glm::mat4* dfVue::nextFrame(time_t t)
{
	static glm::mat4x4 matrice;

	m_currentFrame += t;

	float frame = m_currentFrame * m_fps / 1000.0f;
	if (frame + 1 >= m_animations.size()) {
		return nullptr;
	}

	// interpolate the world position
	float f;
	float m = modf(frame, &f);

	glm::vec3 translation = glm::mix(m_animations[(int)f].m_position, m_animations[(int)f + 1].m_position, m);
	glm::vec3 rotation = glm::mix(m_animations[(int)f].m_rotation, m_animations[(int)f + 1].m_rotation, m);
	glm::vec3 scale = glm::mix(m_animations[(int)f].m_scale, m_animations[(int)f + 1].m_scale, m);

	glm::mat4 rMatrix = glm::eulerAngleXYX(rotation.x, rotation.y, rotation.z);	// inverse pitch and yaw 
	glm::mat4 sMatrix = glm::scale(scale / 10.0f);
	glm::mat4 tMatrix = glm::translate(translation / 10.0f);

	matrice = tMatrix * sMatrix * rMatrix;

	return &matrice;
}

/**
 * return the time of the current frame
 */
time_t dfVue::currentFrame(void)
{
	return m_currentFrame;
}

/**
 * set a new frame time
 */
void dfVue::currentFrame(time_t t)
{
	m_currentFrame = t;
	nextFrame(0);
}

dfVue::~dfVue()
{
}
