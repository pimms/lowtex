#pragma once

#include "node.h"


enum TokType
{
	CMD_BEG,
	CMD_CNT,
	CMD_END,
	RAW,
};

std::string tokType2str(TokType t);

struct Token
{
	TokType type;
	std::string str;
};


class Parser
{
public:
	Parser(std::string texInput);

	NodeVector parse();
	std::vector<Token> tokenize();

protected:
	void spacifyInput();
	std::vector<std::string> splitInput();

	bool validCommand(std::string cmd);

private:
	std::string _input;
};
