#pragma once

#include <string>
#include <map>

class fwTexture;

namespace DarkForces {

	class FNT {
#pragma pack(push)
#pragma pack(1)
		struct fntChar {
			uint8_t width;
			uint8_t bitmap[1];
		};

		struct fntHeader {
			char magic[4];		// // 'FNT' + 15h (21d)
			uint8_t height;
			uint8_t u1;
			uint16_t size;
			uint8_t first;
			uint8_t last;
			uint8_t pad[22];
			fntChar chars[1];
		};
#pragma pack(pop)

		struct Character {
			uint32_t m_width;
			uint8_t m_bitmap;
		};
		uint32_t m_height = 0;
		uint32_t m_firstChar = 0;
		uint32_t m_lastChar = 0;
		std::map<char, fntChar*> m_bitmaps;

	public:
		FNT(const std::string& file);
		void draw(fwTexture* texture, const std::string& text);

		static void draw(fwTexture* texture, const std::string& text, const std::string& font);
	};
}