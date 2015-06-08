#pragma once

#include <vector>
#include <deque>
#include <string>

struct Box
{
	Box() { x=y=w=h=0; }
	int x;
	int y;
	int w;
	int h;
};

enum YAlign
{
	BELOW_ZERO = 0x913485,
	ABOVE_ZERO = 0x913486,

	BOT_AT_ZERO = 0x913487,
	TOP_AT_ZERO = 0x913488,
};

class Surface
{
public:
	Surface();

	void addChar(int x, int y, char ch);
	void addString(int x, int y, std::string str);

	/**
	 * Add a surface to this surface. The yAlign parameter can
	 * either be a constant value, or an enumerated value from the
	 * YAlign enum. The Y-axis of the sub-surface will be placed
	 * on the "yAlign"-coordinate for non-enum values.
	 */
	void addSurface(int x, Surface &surface, int yAlign=0);

	const Box& getBox() const;

	void print();

private:
	void requestCoord(int x, int y);
	void updateBox();

	std::vector<char>& getRow(int y);

	Box _box;
	std::deque<std::pair<int,std::vector<char>>> _tex;
};
