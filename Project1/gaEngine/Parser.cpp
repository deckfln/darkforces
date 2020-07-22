#include "Parser.h"

#include <stdio.h>
#include <list>

#include "gaDebug.h"
#include <iostream>

using namespace GameEngine;

/**
 * count the depth of each rule
 * build from bottom to top: record the parent rule of each rule
 */
size_t Parser::count(uint32_t expression, size_t depth)
{
	size_t max_depth = depth;
	size_t d = depth;

	if (m_mapRules.count(expression) == 0) {
		return depth;
	}

	for (auto rule : m_mapRules[expression]) {
		for (size_t i = 0; i < rule->m_expressions.size(); i++) {
			d = count(rule->m_expressions[i], depth + 1);
			if (d > max_depth) {
				max_depth = d;
			}
		}
	}
	return max_depth;
}

/**
 * build a tree of rules
 * the deeper a rule the more priority it get
 */
void Parser::buildTreeRule(void) 
{
	// build from top to bottom: record the parent rule of each rule
	for (auto& rule : m_grammar) {
		m_mapRules[rule.m_expression].push_back(&rule);
	}

	for (auto& rule : m_grammar) {
		int depth = count(rule.m_expression, 0);

		if (depth > rule.m_depth) {
			rule.m_depth = depth;
		}
	}
}

/**
 * Apply a set of rules on the list until none can be applied
 */
void GameEngine::Parser::applyRules(std::vector<ParserRule*>& rules)
{
	//build a reverse lookup by first expression of the rules

	std::map<uint32_t, std::list<ParserRule*>> byFirstExpression;

	for (auto& rule : rules) {
		switch (rule->m_and_or) {
		case PARSER_AND: {
			uint32_t code = rule->m_expressions[0];
			byFirstExpression[code].push_back(rule);
			break; }
		case PARSER_OR: {
			for (size_t i = 0; i < rule->m_expressions.size(); i++) {
				byFirstExpression[rule->m_expressions[i]].push_back(rule);
			}
			break; }
		case PARSER_LIST:
			byFirstExpression[rule->m_expressions[0]].push_back(rule);
			break;
		}
	}

	// start parsing

	struct ParserExpression* current_expression;
	struct ParserExpression* next_expression;
	uint32_t code;

	// loop over the expressions until none get replaced
	bool applyRules = true;
	bool foundRule = true;

	std::vector<ParserExpression>* tmp = nullptr;
	size_t indexA = 0;
	size_t indexB = 0;

	while (applyRules) {
		applyRules = false;

		// clean the target
		indexB = 0;

		for (indexA = 0; indexA < m_sizeA; indexA++) {
			foundRule = false;

			current_expression = &m_pListA->at(indexA);
			code = current_expression->m_expression;

			// the code is start of a rule
			if (byFirstExpression.count(code) != 0) {
				std::list<ParserRule*> rules = byFirstExpression[code];

				// test all compatible rules
				for (auto rule : rules) {
					bool result = true;

					next_expression = &m_pListA->at(indexA + 1);

					switch (rule->m_and_or) {
					case PARSER_AND:
						// test all components
						for (size_t i = 1; i < rule->m_expressions.size(); i++) {
							if (indexA + i == m_pListA->size()) {
								break;
							}
							result &= m_pListA->at(indexA + i).m_expression == rule->m_expressions[i];
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

						m_pListB->at(indexB).m_expression = rule->m_expression;
						m_pListB->at(indexB).m_token = nullptr;
						m_pListB->at(indexB).m_children.clear();

						// record the children of the expression
						switch (rule->m_and_or) {
						case PARSER_AND:
							// all elements need to be accounted for
							for (size_t i = 0; i < rule->m_expressions.size(); i++) {
								m_pListB->at(indexB).m_children.push_back(m_pListA->at(indexA + i));
							}
							indexA += (rule->m_expressions.size() - 1);
							break;
						case PARSER_OR:
							// only the current element is accounted for
							m_pListB->at(indexB).m_children.push_back(m_pListA->at(indexA));
							break;
						case PARSER_LIST:
							while (indexA < m_sizeA && m_pListA->at(indexA).m_expression == code) {
								m_pListB->at(indexB).m_children.push_back(m_pListA->at(indexA));
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
			else if (m_lists.count(code) > 0) {
				// merge list of list
				size_t start = indexA;
				while (indexA < m_sizeA && m_pListA->at(indexA + 1).m_expression == code) {
					for (size_t j = 0; j < m_pListA->at(indexA + 1).m_children.size(); j++) {
						m_pListA->at(start).m_children.push_back(m_pListA->at(indexA + 1).m_children[j]);
					}
					indexA++;
					applyRules = true;
				}
				m_pListB->at(indexB) = m_pListA->at(start);
				indexB++;
				foundRule = true;
			}

			if (!foundRule) {
				//copy the expression to listB
				m_pListB->at(indexB) = m_pListA->at(indexA);
				indexB++;
			}

			// add the rule to listB
			if (indexB == m_pListB->size()) {
				m_pListB->resize(indexB + 1);

			}
		};

		// swap the lists
		tmp = m_pListA;
		m_pListA = m_pListB;
		m_pListB = tmp;

		m_sizeA = indexB;
	}
}

/**
 *
 */
Parser::Parser(Lexer& lexer, 
	std::vector<ParserRule>& grammar, 
	std::map<std::string, uint32_t>& keywords):
	m_grammar(grammar)
{
	// build a tree of rules
	buildTreeRule();

	// sort the rules by depth and deal with the deepest first
	int max_depth = 0;
	for (auto& rule : m_grammar) {
		m_mapRulesByDepth[rule.m_depth].push_back(&rule);
		if (rule.m_depth > max_depth) {
			max_depth = rule.m_depth;
		}
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

	// init list A & B
	m_listB.resize(1);

	m_pListA = &m_listA;
	m_pListB = &m_listB;
	m_sizeA = m_pListA->size();

	// apply the rules by order of depth
	for (int i = 0; i <= max_depth; i++) {
		if (m_mapRulesByDepth.count(i) > 0) {
			applyRules(m_mapRulesByDepth[i]);
		}
	}
}

/**
 * return the next expression
 */
ParserExpression* GameEngine::Parser::next(void)
{
	if (m_current >= m_sizeA) {
		return nullptr;
	}

	return &m_pListA->at(m_current++);
}
