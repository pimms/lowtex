#include "node.h"

Node::Node(NodeType t, std::string s):
	_type(t),
	_str(s)
{

}

NodeType Node::getType() const
{
	return _type;
}

std::string Node::getText() const
{
	return _str;
}

NodeVector& Node::getChildren(int subgrp)
{
	return _children[subgrp];
}

void Node::addChild(NodePtr node, int subgrp)
{
	if (_type == TEXT)
		throw std::runtime_error("Unable to add child to TEXT-node");
	_children[subgrp].push_back(node);
}

void Node::absorb(NodeVector vec, int subgrp)
{
	_children[subgrp].insert(_children[subgrp].end(),
							 vec.begin(),
							 vec.end());
}

void Node::print()
{
	if (_type == TEXT) {
		printf("(%s)", _str.c_str());
	} else {
		printf("%s{", _str.c_str());
		for (auto child : _children[0])
			child->print();
		printf("}");

		if (_children[1].size()) {
			printf("{");
			for (auto child : _children[1])
				child->print();
			printf("}");
		}
	}
}


void TmpNode::setParent(NodePtr parent)
{
	_parent = parent;
}

NodePtr TmpNode::getParent()
{
	return _parent;
}



static int surfMaxW(Surface &s1, Surface &s2)
{
	return std::max(s1.getBox().w, s2.getBox().w);
}

static void surfRightPad(Surface &surf, int pad)
{
	int w = surf.getBox().w;
	surf.addChar(w + pad - 1, 0, ' ');
}


static Surface renderFrac(NodePtr node)
{
	Surface surf, numerator, denominator;

	numerator = render(node->getChildren(0));
	denominator = render(node->getChildren(1));

	const int maxW = surfMaxW(numerator, denominator);
	int numX = (maxW / 2) - (numerator.getBox().w / 2);
	int denX = (maxW / 2) - (denominator.getBox().w / 2);

	surf.addSurface(numX, numerator, YAlign::ABOVE_ZERO);
	surf.addSurface(denX, denominator, YAlign::BELOW_ZERO);

	for (int i=0; i<maxW; i++) {
		surf.addChar(i, 0, '-');
	}

	return surf;
}

static Surface renderSqrt(NodePtr node)
{
	Surface surf, sub;

	sub = render(node->getChildren());

	surf.addChar(0, 0, 'V');

	int i;
	for (i = 1; i < sub.getBox().h; i++)
		surf.addChar(i, i, '/');

	surf.addSurface(i, sub, YAlign::BOT_AT_ZERO);

	for (int j=0; j<sub.getBox().w; j++) {
		surf.addChar(j+i, sub.getBox().h, '-');
	}

	return surf;
}

static Surface renderRoot(NodePtr node)
{
	Surface surf;
	Surface root;
	Surface radical;

	root = render(node->getChildren(0));

	NodePtr fakeSqrt = std::make_shared<Node>(CMD, "sqrt");
	fakeSqrt->absorb(node->getChildren(1), 0);
	radical = renderSqrt(fakeSqrt);

	surf.addSurface(root.getBox().w - 1, radical);
	surf.addSurface(0, root, 1);
	return surf;
}

static Surface renderShiftedY(NodePtr node, int y)
{
	Surface surf;

	Surface sub;
	sub = render(node->getChildren());

	surf.addSurface(0, sub, y);
	return surf;
}

Surface render(NodeVector nodes)
{
	Surface surf;
	int chx = 0;

	int i=0;
	while (i < nodes.size()) {
		NodePtr node = nodes[i];

		// Check for special case where sub- and super-script
		// preceed one another (in either  order)
		if (i+1 < nodes.size()) {
			NodePtr next = nodes[i+1];

			if ((next->getText() == "^" || next->getText() == "_") &&
				(node->getText() == "^" || node->getText() == "_") &&
				node->getText() != next->getText()) {
				Surface s1, s2;
				s1 = render(node);
				s2 = render(next);
				surf.addSurface(chx, s1);
				surf.addSurface(chx, s2);
				chx += surfMaxW(s1, s2);
				i += 2;
				continue;
			}
		}

		Surface sub = render(node);
		surf.addSurface(chx, sub);
		chx += sub.getBox().w;

		i++;
	}

	return surf;
}

Surface render(NodePtr node)
{
	Surface surf;
	int chx = 0;

	if (node->getType() == NodeType::TEXT ||
		node->getText() == "pi") {
		surf.addString(0, 0, node->getText());
	} else if (node->getText() == "frac") {
		surf = renderFrac(node);
	} else if (node->getText() == "sqrt") {
		surf = renderSqrt(node);
	} else if (node->getText() == "root") {
		surf = renderRoot(node);
	} else if (node->getText() == "^") {
		surf = renderShiftedY(node, 1);
	} else if (node->getText() == "_") {
		surf = renderShiftedY(node, -1);
	}

	return surf;
}
