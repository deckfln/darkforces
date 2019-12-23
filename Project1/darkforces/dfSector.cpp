#include "dfSector.h"

#include <iostream>
#include <glm/glm.hpp>

dfSector::dfSector(std::fstream &infile)
{
	int nbVertices;
	int currentVertice = 0;
	glm::vec2* vertices = nullptr;

	int nbWalls;
	int currentWall = 0;
	glm::vec2** walls_left = nullptr;
	glm::vec2** walls_right = nullptr;

	std::string line, dump;
	std::istringstream iss(line);

	while (std::getline(infile, line))
	{
		if (line.find("NAME") != std::string::npos) {
			iss = line;
			iss >> dump >> m_name;
		}
		else if (line.find("AMBIENT") != std::string::npos) {
			iss = line;
			iss >> dump >> m_ambient;
		}
		else if (line.find("FLOOR TEXTURE") != std::string::npos) {
			// PASS
		}
		else if (line.find("FLOOR ALTITUDE") != std::string::npos) {
			m_floorAltitude = std::stof(line.substr(20));
		}
		else if (line.find("CEILING TEXTURE") != std::string::npos) {
			//PASS
		}
		else if (line.find("CEILING ALTITUDE") != std::string::npos) {
			m_ceilingAltitute = std::stof(line.substr(20));
		}
		else if (line.find("SECOND ALTITUDE") != std::string::npos) {
			//PASS
		}
		else if (line.find("SECOND ALTITUDE") != std::string::npos) {
			//PASS
		}
		else if (line.find("VERTICES") != std::string::npos) {
			nbVertices = std::stoi(line.substr(11));
			vertices = new glm::vec2[nbVertices];
		}
		else if (line.find("X:") != std::string::npos) {
			float x  = std::stoi(line.substr(7, 6));
			float z = std::stoi(line.substr(20, 7));

			vertices[currentVertice++] = glm::vec2(x, z);
		}
		else if (line.find("WALLS") != std::string::npos) {
			nbWalls = std::stoi(line.substr(7, 3));
			walls_left = new glm::vec2*[nbWalls];
			walls_right = new glm::vec2 * [nbWalls];
		}
		else if (line.find("WALL LEFT") != std::string::npos) {
			int left = std::stoi(line.substr(14, 4));
			int right = std::stoi(line.substr(26, 4));

			walls_left[currentWall] = vertices + left;
			walls_right[currentWall] = vertices + right;

			currentWall++;
		}
		else if (line.find("S e c t o r   D e f i n i t i o n") != std::string::npos) {
			break;
		}
	}

	delete[] vertices;
	delete[] walls;
}

~dfSector()
{

}