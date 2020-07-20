#include "Parser.h"

#include <stdio.h>
#include <list>

#include "gaDebug.h"
#include <iostream>

using namespace GameEngine;

Parser::Parser(Lexer& lexer, 
	std::map<uint32_t, ParserGrammar>& grammar, 
	std::map<std::string, uint32_t>& keywords):
	m_grammar(grammar)
{
	std::map<uint32_t, ParserGrammar*> lists;
	
		// build a reverse lookup by first code
	// build a reverse lookup by list

	std::map<uint32_t, std::list<ParserGrammar*>> m_byFirstExpression;
	for (auto& kw : m_grammar) {
		struct ParserGrammar* rule = &kw.second;

		switch (rule->m_and_or) {
		case PARSER_AND: {
			uint32_t code = rule->m_expressions[0];
			m_byFirstExpression[code].push_back(rule);
			break; }
		case PARSER_OR: {
			for (size_t i = 0; i < rule->m_nbExpressions; i++) {
				m_byFirstExpression[rule->m_expressions[i]].push_back(rule);
			}
			break; }
		case PARSER_LIST:
			m_byFirstExpression[rule->m_expressions[0]].push_back(rule);
			lists[rule->m_expression] = rule;
			break;
		}
	}
	// extend the grammar with the parser basics
	grammar[STRING] = { STRING, PARSER_SINGLE, 0 };
	grammar[COLON] = { COLON, PARSER_SINGLE, 0 };
	grammar[DIGIT] = { DIGIT, PARSER_SINGLE, 0 };

	// extend the grammar by adding the keywords
	for (auto& kw : keywords) {
		grammar[kw.second] = { kw.second, PARSER_SINGLE, 0 };
	}

	// create the first list of expressions
	const struct token* token;

	m_listA.resize(lexer.size());
	size_t i=0;

	while (token = lexer.next()) {
		m_listA[i].m_expression = token->m_code;
		m_listA[i].m_token = token;
		i++;
	}

	struct ParserExpression* current_expression;
	struct ParserExpression* next_expression;
	uint32_t code;

	// loop over the expressions until none get replaced
	bool applyRules = true;
	bool foundRule = true;

	m_listB.resize(1);

	std::vector<ParserExpression> *listA = &m_listA;
	std::vector<ParserExpression> *listB = &m_listB;
	std::vector<ParserExpression>* tmp = nullptr;
	size_t indexA = 0;
	size_t indexB = 0;
	size_t sizeA = listA->size();

	while (applyRules) {
		applyRules = false;

		// clean the target
		indexB = 0;

		for (indexA = 0; indexA < sizeA; indexA++) {
			foundRule = false;

			current_expression = &listA->at(indexA);
			code = current_expression->m_expression;

			if (code == 1066) {
				printf("");
			}
			// the code is start of a rule
			if (m_byFirstExpression.count(code) != 0) {
				std::list<ParserGrammar*> rules = m_byFirstExpression[code];

				// test all compatible rules
				for (auto rule : rules) {
					bool result = true;

					next_expression = &listA->at(indexA + 1);

					switch (rule->m_and_or) {
					case PARSER_AND:
						// test all components
						for (size_t i = 1; i < rule->m_nbExpressions; i++) {
							if (indexA + i == listA->size()) {
								break;
							}
							result &= listA->at(indexA + i).m_expression == rule->m_expressions[i];
						}
						break;

					case PARSER_OR:
						// or tokens have been split earlier
						result = true;
						break;

					case PARSER_LIST:
						// a list of similar elements;
						result = true;
						break;
					}

					// do we have a matching expression
					if (result) {

						listB->at(indexB).m_expression = rule->m_expression;
						listB->at(indexB).m_token = nullptr;
						listB->at(indexB).m_children.clear();

						// record the children of the expression
						switch (rule->m_and_or) {
						case PARSER_AND:
							// all elements need to be accounted for
							for (size_t i = 0; i < rule->m_nbExpressions; i++) {
								listB->at(indexB).m_children.push_back(listA->at(indexA + i));
							}
							indexA += (rule->m_nbExpressions - 1);
							break;
						case PARSER_OR:
							// only the current element is accounted for
							listB->at(indexB).m_children.push_back(listA->at(indexA));
							break;
						case PARSER_LIST:
							while (indexA < sizeA && listA->at(indexA).m_expression == code) {
								listB->at(indexB).m_children.push_back(listA->at(indexA));
								indexA++;
							}
							indexA--;
							break;
						}
						indexB++;
						applyRules = foundRule = true;

						break;	// no need to test the other rules
					}
				}
			}	// END the code is start of a rule
			else if (lists.count(code) > 0) {
				// merge list of list
				size_t start = indexA;
				while (indexA < sizeA && listA->at(indexA + 1).m_expression == code) {
					for (size_t j = 0; j < listA->at(indexA + 1).m_children.size(); j++) {
						listA->at(start).m_children.push_back(listA->at(indexA + 1).m_children[j]);
					}
					indexA++;
					applyRules = true;
				}
				listB->at(indexB) = listA->at(start);
				indexB++;
				foundRule = true;
			}

			if (!foundRule) {
				//copy the expression to listB
				listB->at(indexB) = listA->at(indexA);
				indexB++;
			}

			// add the rule to listB
			if (indexB == listB->size()) {
				listB->resize(indexB + 1);

			}
		};

		// swap the lists
		tmp = listA;
		listA = listB;
		listB = tmp;

		sizeA = indexB;

		std::cout << "******************************************************************" << std::endl;
		for (size_t indexA = 0; indexA < listA->size(); indexA++) {
			std::cout << listA->at(indexA).m_expression << std::endl;
		};
	}
}
