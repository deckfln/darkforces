#pragma once

#include <string>
#include <vector>
#include <list>

#include "Lexer.h"

namespace GameEngine
{
	enum {
		PARSER_AND,
		PARSER_OR,
		PARSER_LIST,
		PARSER_SINGLE
	};

	struct ParserRule {
		uint32_t m_expression;
		uint32_t m_and_or;
		std::vector<uint16_t> m_expressions;
		size_t m_depth = 0;
	};

	struct ParserExpression {
		uint32_t m_expression;	// expression code
		uint32_t m_pos;			// position in the stream

		const struct token* m_token = nullptr;
		std::vector<struct ParserExpression> m_children;
	};

	class Parser
	{
		std::vector<ParserRule>& m_grammar;
		std::vector<ParserExpression> m_listA;
		std::vector<ParserExpression> *m_pListA;

		std::vector<ParserExpression> m_listB;
		std::vector<ParserExpression> *m_pListB;
		size_t m_current = 0;
		size_t m_sizeA = 0;

		std::map<uint32_t, ParserRule*> m_lists;
		std::map<uint32_t, std::vector<ParserRule*>> m_mapRules;
		std::map<uint32_t, std::vector<ParserRule*>> m_mapRulesByDepth;

		size_t count(uint32_t expression, size_t depth);
		void buildTreeRule(void);
		void applyRules(std::vector<ParserRule*>& rules);

	public:
		Parser(Lexer& lexer, 
			std::vector<ParserRule>& grammar,
			std::map<std::string, uint32_t>& keywords);
		ParserExpression* next(void);
	};
}