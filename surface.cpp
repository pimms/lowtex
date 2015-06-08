#include "surface.h"
#include <stdexcept>

Surface::Surface()
{
	requestCoord(0, 0);
}

void Surface::addChar(int x, int y, char ch)
{
	requestCoord(x, y);

	std::vector<char>& row = getRow(y);

	row[x] = ch;
}

void Surface::addString(int x, int y, std::string str)
{
	for (char ch : str)
		addChar(x++, y, ch);
}

void Surface::addSurface(int x, Surface &surface, int yAlign)
{
	if (yAlign == BELOW_ZERO)
		yAlign = -surface._box.h;
	if (yAlign == ABOVE_ZERO)
		yAlign = (-surface._box.y) + 1;
	if (yAlign == BOT_AT_ZERO)
		yAlign = -surface._box.y;
	if (yAlign == TOP_AT_ZERO)
		yAlign = (-surface._box.h) + 1;

	auto it = surface._tex.rbegin();
	for (; it != surface._tex.rend(); it++) {
		int dx = 0;
		for (char ch : it->second) {
			addChar(x + dx++, yAlign + it->first, ch);
		}
	}
}

const Box& Surface::getBox() const
{
	return _box;
}

void Surface::print()
{
	auto it = _tex.rbegin();
	for (; it != _tex.rend(); it++) {
		for (auto ch : it->second)
			printf("%c", ch);
		printf("\n");
	}
}

void Surface::requestCoord(int x, int y)
{
	std::pair<int,std::vector<char>> row;

	// If the texture is empty
	if (_tex.empty()) {
		_tex.insert(_tex.begin(),
				    std::make_pair(y, std::vector<char>()));
	}

	// For when the requested Y-coordinate is on a line below
	// what is currently defined.
	const int prevLow = _tex.begin()->first;
	int insPos = 0;
	for (int i=y; i<prevLow; i++) {
		_tex.insert(_tex.begin() + (insPos++),
				    std::make_pair(i, std::vector<char>()));
	}

	// For when the requested Y-coordinate is on a line above
	// what is currently defined.
	auto iterator = (_tex.end() - 1);
	for (int i = iterator->first+1; i<=y; i++) {
		_tex.push_back(std::make_pair(i, std::vector<char>()));
	}

	std::vector<char>& chRow = getRow(y);
	if (chRow.size() < x + 1)
		chRow.resize(x+1, ' ');

	updateBox();
}

void Surface::updateBox()
{
	if (_tex.empty()) {
		_box = Box();
		return;
	}

	_box.x = 0;
	_box.y = _tex.front().first;
	_box.h = _tex.size();
	_box.w = -1;

	for (auto pair : _tex) {
		_box.w = std::max(_box.w, (int)pair.second.size());
	}

	for (auto &pair : _tex) {
		pair.second.resize(_box.w, ' ');
	}
}

std::vector<char>& Surface::getRow(int y)
{
	for (auto &pair : _tex) {
		if (pair.first == y) {
			return pair.second;
		}
	}

	throw std::runtime_error("Surface::getRow(): Unable to find row");
}
