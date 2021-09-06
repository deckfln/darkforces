#pragma once

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <map>

class fwTexture;

namespace DarkForces {
	struct LFD_index {
		uint32_t start;
		uint32_t len;
	};

	class FileLFD
	{
		std::ifstream fd;
		std::map<std::string, LFD_index> m_index;
		std::map<std::string, void*> m_ressources;

		fwTexture* parseDELT(char* buffer, uint32_t size);
		void* parsePLTT(char* buffer, uint32_t size);

	public:
		FileLFD(const std::string& file);
		char* load(const std::string& file, int& size);
		~FileLFD();
	};
};
