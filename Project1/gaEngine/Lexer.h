#pragma once

#include <string>
#include <map>
#include <queue>

namespace GameEngine
{
	enum {
		STRING,
		COLON,
		DIGIT,
		QUOTE
	};

	struct token {
		uint32_t m_code;	
		uint32_t m_position;	// position in the stream

		std::string m_tvalue;
		float m_vvalue=0.0f;

		token(uint32_t i, uint32_t c) {
			m_code = c;
			m_position = i;
		}
		token(uint32_t i, uint32_t c, const std::string& str) {
			m_code = c;
			m_tvalue = str;
			m_position = i;
		}
		token(uint32_t i, uint32_t c, float val) {
			m_code = c;
			m_vvalue = val;
			m_position = i;
		}
	};

	class Lexer
	{
		const std::string& m_source;
		std::map<std::string, uint32_t>& m_keywords;
		std::vector<struct token> m_tokens;

		uint32_t m_counter = 0;

		void gettoken(int start, int end);

	public:
		Lexer(const std::string& tst, std::map<std::string, uint32_t>& keywords);
		const struct token* next(void);
		size_t size(void) { return m_tokens.size(); };
		~Lexer();
	};
}