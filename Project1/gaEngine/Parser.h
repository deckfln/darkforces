#pragma once

#include <string>
#include <vector>

#include "Lexer.h"

namespace GameEngine
{
	enum {
		PARSER_AND,
		PARSER_OR,
		PARSER_LIST,
		PARSER_SINGLE
	};

	struct ParserGrammar {
		uint32_t m_expression;
		uint32_t m_and_or;
		uint32_t m_nbExpressions;
		uint16_t m_expressions[32];

		struct ParserGrammar* m_children[32];
		struct ParserGrammar* m_parent=nullptr;
	};

	struct ParserExpression {
		uint32_t m_expression;	// expression code
		uint32_t m_pos;			// position in the stream

		const struct token* m_token = nullptr;
		std::vector<struct ParserExpression> m_children;
	};

	class Parser
	{
		std::map<uint32_t, ParserGrammar>& m_grammar;
		std::vector<ParserExpression> m_listA;
		std::vector<ParserExpression> m_listB;

	public:
		Parser(Lexer& lexer, 
			std::map<uint32_t, ParserGrammar>& grammar,
			std::map<std::string, uint32_t>& keywords);
	};
}