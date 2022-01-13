#pragma once

#include <string>
#include <vector>

class dfFileSystem;
class alSound;

class dfVOC
{
	std::string m_name;
	std::vector<unsigned char> m_pcm8;
	unsigned short m_repeat = 0;
	unsigned short m_markerID = 0;
	unsigned long m_sampleRate = 0;
	alSound* m_sound = nullptr;

public:
	dfVOC(dfFileSystem* fs, const std::string& file);
	const std::string& name(void) { return m_name; };
	alSound* sound(void);
	~dfVOC();
};

dfVOC* loadVOC(const std::string& name);	// load a cached file

namespace DarkForces {
	void deleteVOCcache(void);						// clean up cached files
}
