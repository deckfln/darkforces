#pragma once

#include <string>
#include <vector>
class dfFileSystem;

class dfVOC
{
	std::vector<unsigned char> m_sound;
	float m_sampleRate = 0;
	unsigned short m_repeat = 0;
	unsigned short m_markerID = 0;

public:
	dfVOC(dfFileSystem* fs, const std::string& file);
	~dfVOC();
};