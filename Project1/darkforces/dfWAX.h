#pragma once
#include <string>
#include <vector>

class dfFileSystem;
class dfFME;
class dfPalette;

enum dfWaxMode {
	DF_WAX_ENEMY,
	DF_WAX_SCENERY,
	DF_WAX_BARREL
};

struct dfWaxAnimation {
	std::vector<dfFME*> m_frames;
};

struct dfWaxAngles {
	std::vector<dfWaxAnimation *> m_animations;
};

class dfWAX
{
	std::string m_name;
	void* m_data = nullptr;

	int m_nbstates = 0;
	std::vector<dfWaxAngles *> m_states;

public:
	dfWAX(dfFileSystem* fs, dfPalette *palette, std::string& name);
	~dfWAX();
};