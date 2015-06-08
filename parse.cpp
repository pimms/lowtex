#include "parse.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <deque>

const std::vector<char> g_spec =
	{'{', '}', '_', '^', '\\', '(', ')'};
const std::vector<std::string> g_cmds = {
	"frac",
	"sqrt",
	"root",
	"pi"
};


std::string tokType2str(TokType t)
{
	switch (t) {
		case CMD_BEG:
			return "CMDBEG";
		case CMD_CNT:
			return "CMDCNT";
		case CMD_END:
			return "CMDEND";
		case RAW:
			return "RAW";
	}

	return "(-?-)";
}


Parser::Parser(std::string texInput):
	_input(texInput)
{ }

NodeVector Parser::parse()
{
	std::vector<Token> tokens = tokenize();
	std::deque<NodePtr> stack;
	stack.push_back(std::make_shared<Node>(CMD, "ROOT"));
	NodePtr lastCompleted = nullptr;

	while (tokens.size() && stack.size()) {
		NodePtr node;
		Token tok = tokens.front();
		tokens.erase(tokens.begin());

		if (tok.type == TokType::RAW) {
			node = std::make_shared<Node>(TEXT, tok.str);
			stack.back()->addChild(node);
		} else if (tok.type == TokType::CMD_BEG) {
			node = std::make_shared<Node>(CMD, tok.str);
			stack.push_back(node);
		} else if (tok.type == TokType::CMD_CNT) {
			node = std::make_shared<TmpNode>(TMP, "");
			((TmpNode*)node.get())->setParent(lastCompleted);
			stack.push_back(node);
		} else if (tok.type == TokType::CMD_END) {
			node = stack.back();
			stack.pop_back();
			if (!stack.size())
				throw std::runtime_error("Too many CMD_ENDs");

			if (node->getType() == TMP) {
				if (!lastCompleted)
					throw std::runtime_error("Noone to absorb children");
				NodePtr parent = ((TmpNode*)node.get())->getParent();
				parent->absorb(node->getChildren(), 1);
			} else {
				if (node->getType() == CMD)
					lastCompleted = node;
				stack.back()->addChild(node);
			}
		}
	}

	if (tokens.size())
		throw std::runtime_error("Unconsomed tokens");
	if (stack.size() != 1)
		throw std::runtime_error("Invalid format - !1 stack elems");

	NodeVector vec = stack.front()->getChildren(0);
	return vec;
}

std::vector<Token> Parser::tokenize()
{
	std::vector<Token> tokens;

	spacifyInput();
	std::vector<std::string> words = splitInput();

	int i = 0;
	while (i < words.size()) {
		if (words[i] == "\\") {
			if (i+1 >= words.size())
				throw std::runtime_error("Invalid token");

			if (words[i+1] == "pi") {
				tokens.push_back(Token{CMD_BEG, "pi"});
				tokens.push_back(Token{CMD_END, ""});
				i += 2;
			} else {
				if (i+2 >= words.size() ||
					words[i+2] != "{" ||
					!validCommand(words[i+1]))
					throw std::runtime_error("Invalid token");
				tokens.push_back(Token{CMD_BEG, words[i+1]});
				i += 3;
			}
		} else if (words[i] == "{") {
			tokens.push_back(Token{CMD_CNT, ""});
			i++;
		} else if (words[i] == "}") {
			tokens.push_back(Token{CMD_END, ""});
			i++;
		} else if (words[i] == "^" || words[i] == "_") {
			if (i+1 >= words.size() || words[i+1] != "{")
				throw std::runtime_error("Invalid token");
			tokens.push_back(Token{CMD_BEG, words[i]});
			i += 2;
		} else {
			/* Default case: treat word as raw token */
			tokens.push_back(Token{RAW, words[i]});
			i++;
		}
	}

	return tokens;
}

void Parser::spacifyInput()
{
	// Insert spaces surrounding the special
	// character {, }, _, ^, and \.

	for (int i=0; i<_input.size(); i++) {
		if (std::find(g_spec.begin(), g_spec.end(), _input[i]) != g_spec.end()) {
			// Insert before
			if (i >= 1 && _input[i-1] != ' ') {
				_input.insert(_input.begin()+(i), ' ');
				i++;
			}

			// Insert after
			if (i + 1 < _input.size() && _input[i+1] != ' ') {
				_input.insert(_input.begin()+(i+1), ' ');
				i++;
			}
		}
	}
}

std::vector<std::string> Parser::splitInput()
{
	std::istringstream iss(_input);

	std::vector<std::string> words;
	std::copy(std::istream_iterator<std::string>(iss),
			  std::istream_iterator<std::string>(),
			  std::back_inserter(words));

	return words;
}

bool Parser::validCommand(std::string cmd)
{
	return std::find(g_cmds.begin(), g_cmds.end(), cmd) != g_cmds.end();
}
