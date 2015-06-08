#include <iostream>
#include "node.h"
#include "parse.h"
#include "surface.h"

const std::string testTex = "x = 4";

int main()
{
	std::string input;
	while (true) {
		printf("Feed me LaTeX! (q to quit): ");
		std::getline(std::cin, input);
		if (input == "q")
			break;

		Parser parser(input);
		NodeVector nodes = parser.parse();
		Surface surf = render(nodes);
		surf.print();
	}

	return 0;
}

/**
 * Various test routines. Nothing of interest occurs here.
 */

void testNodePrint()
{
	printf("testNodePrint():\n");
	std::shared_ptr<Node> root = std::make_shared<Node>(CMD, "root");
	root->addChild(std::make_shared<Node>(TEXT, "hei sveis gutter"));
	root->addChild(std::make_shared<Node>(TEXT, "jada, kult dette"));

	std::shared_ptr<Node> child = std::make_shared<Node>(CMD, "sub");
	child->addChild(std::make_shared<Node>(TEXT, "texti text"));
	child->addChild(std::make_shared<Node>(CMD, "subsub"));
	root->addChild(child);

	root->print();
	printf("\n\n");
}

void testTokenization()
{
	printf("testTokenization():\n");

	Parser parser(testTex);

	std::vector<Token> tokens = parser.tokenize();

	for (Token t : tokens) {
		printf("[%s]", tokType2str(t.type).c_str());
		if (t.str.length())
			printf("(%s)", t.str.c_str());
		printf("\n");
	}

	printf("\n\n");
}

void testParse()
{
	printf("testParse():\n");
	Parser parser(testTex);
	NodeVector nodes = parser.parse();

	for (NodePtr node : nodes)
		node->print();

	printf("\n\n");
}

void testSurface()
{
	printf("testSurface():\n");

	printf("\nsimple surface:\n");
	Surface surf;
	surf.addString(1, 0, "sub");
	surf.addChar(0, 0, '|');
	surf.addChar(4, 0, '|');
	surf.addString(0, 1, "+---+");
	surf.addString(0, -1, "+---+");
	surf.print();

	printf("\nsurface containing surface:\n");
	Surface super;
	super.addString(0, 0, "baseline ---");
	super.addSurface(3, surf, -2);
	super.print();

	printf("\nsubsurface alignment testing:\n");
	Surface s1, s2, s3;
	s1.addString(0, 0, "111111");
	s2.addString(0, 0, "222222");
	s2.addString(0, 1, "222222");
	s3.addString(0, 0, "3333333");
	s3.addString(0, -1, "3333333");

	s2.addSurface(6, s3);
	s1.addSurface(6, s2);
	s1.print();
}
