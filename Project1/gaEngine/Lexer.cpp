#include "Lexer.h"

using namespace GameEngine;

/**
 * Convert a string to a keyword
 */
void Lexer::gettoken(int start, int end)
{
	const std::string token = m_source.substr(start, end - start);

	// try to convert to digit
	char* p;
	const char* numToken = token.c_str();
	size_t l = token.size();
	float converted = strtof(numToken, &p);

	if (p != numToken + l) {
		// conversion failed because the input wasn't a number

		// check the keyword provided
		if (m_keywords.count(token) > 0) {
			uint32_t c = m_keywords[token];
			m_tokens.push_back(struct token(start, c));
		}
		else {
			m_tokens.push_back(struct token(start, STRING, token));
		}
	}
	else {
		m_tokens.push_back(struct token(start, DIGIT, converted));
	}
}

Lexer::Lexer(const std::string& tst, std::map<std::string, uint32_t>& keywords) :
	m_source(tst),
	m_keywords(keywords)
{
	size_t start = 0;
	bool spacer = false;
	bool comment = false;
	bool chars = false;

	for (size_t i = 0; i < m_source.size(); i++) {
		char c = tst[i];

		switch (c) {
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			if (!comment) {
				if (chars) {
					gettoken(start, i);
					chars = false;
				}
				spacer = true;
			}
			break;
		case ':':
		case '"':
			if (!comment) {
				if (chars) {
					gettoken(start, i);
					chars = false;
				}

				spacer = false;
				m_tokens.push_back(token(i, (c == ':') ? COLON : QUOTE));
				start = i + 1;
			}
			break;
		case '/':
			if (i + 1 < tst.size()) {
				if (tst[i + 1] == '*') {
					if (chars) {
						gettoken(start, i);
						chars = false;
					}
					comment = true;
					spacer = false;
					i += 1;
				}
			}
			break;
		case '*':
			if (i + 1 < tst.size()) {
				if (tst[i + 1] == '/') {
					comment = false;
					i += 1;
				}
			}
			break;
		default:
			if (!chars && !comment) {
				chars = true;
				spacer = false;
				start = i;
			}
		}
	}

	if (chars) {
		gettoken(start, tst.size());
	}
}

const token* GameEngine::Lexer::next(void)
{
	if (m_counter == m_tokens.size()) {
		return nullptr;
	}

	return &m_tokens[m_counter++];
}

GameEngine::Lexer::~Lexer()
{
}
