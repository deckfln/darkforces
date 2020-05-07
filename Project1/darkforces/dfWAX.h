#pragma once
#include <string>
#include <vector>
#include <map>

class dfFileSystem;
class dfFME;
class dfPalette;
class dfBitmapImage;

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
	std::map<int, dfWaxAnimation*> m_animations;
	std::map<int, dfFME*> m_frames;

public:
	dfWAX(dfFileSystem* fs, dfPalette *palette, std::string& name);
	void getFrames(std::vector<dfBitmapImage*>& m_frames);
	~dfWAX();
};