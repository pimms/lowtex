#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "surface.h"

enum NodeType
{
	TEXT,
	CMD,
	TMP,
};

class Node;
typedef std::shared_ptr<Node> NodePtr;
typedef std::vector<NodePtr> NodeVector;

class Node
{
public:
	Node(NodeType t, std::string s);

	NodeType getType() const;
	std::string getText() const;

	NodeVector& getChildren(int subgrp=0);

	void addChild(NodePtr node, int subgrp=0);
	void absorb(NodeVector vec, int subgrp);

	void print();

private:
	NodeType _type;
	std::string _str;

	NodeVector _children[2];
};

class TmpNode: public Node
{
public:
	TmpNode(NodeType t, std::string s) : Node(t,s) {};
	void setParent(NodePtr parent);
	NodePtr getParent();

private:
	NodePtr _parent;
};


Surface render(NodeVector nodes);
Surface render(NodePtr node);
