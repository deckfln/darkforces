#pragma once

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include <glm/vec2.hpp>
#include "../framework/fwTexture.h"

namespace DarkForces {
#pragma pack(push)
#pragma pack(1)
	struct LFD_index {
		uint32_t start;
		uint32_t len;
	};

	struct PLTT_RGB
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};
#pragma pack(pop)

	class DELT {
		uint32_t m_posX=0;		// position of the picture on the bigger picture
		uint32_t m_posY=0;
		fwTexture* m_texture = nullptr;
	public:
		DELT(uint8_t* buffer, uint32_t size, std::vector<PLTT_RGB>* palette);
		inline fwTexture* texture(void) { return m_texture; };
		inline const glm::ivec2 size(void) { return m_texture->size(); };
		~DELT(void) {
			delete m_texture;
		}
	};

	class ANIM {
		std::vector<DELT*> m_delts;
	public:
		ANIM(uint8_t* buffer, std::vector<PLTT_RGB>* palette);

		inline size_t size(void) {
			return m_delts.size();
		}

		inline DELT* texture(uint32_t i) {
			return m_delts[i];
		}
		~ANIM() {
			for (auto delt : m_delts) {
				delete delt;
			}
		}
	};

	class FileLFD
	{
		std::ifstream m_fd;
		std::map<std::string, LFD_index> m_index;
		std::map<std::string, void*> m_ressources;

		DarkForces::DELT* parseDELT(char* buffer, uint32_t size);
		void* parsePLTT(char* buffer, uint32_t size);

	public:
		FileLFD(const std::string& file);
		DarkForces::ANIM* anim(const std::string& file);
		DarkForces::DELT* delt(const std::string& file);
		void loadPltt(const std::string& file);

		static void load(const std::string& file);										// load a permanent LFD
		static DarkForces::ANIM* loadAnim(const std::string& file, const std::string& source);		// load an ANIM from a permanent LFD
		static DarkForces::DELT* loadDelt(const std::string& file, const std::string& source);		// load an DELT from a permanent LFD
		~FileLFD();
	};
};

extern DarkForces::FileLFD* g_dfMENU;
extern DarkForces::FileLFD* g_dfBRIEF;