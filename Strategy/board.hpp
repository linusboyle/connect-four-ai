#pragma once

#include "point.hpp"
#include "base.hpp"
#include <limits>
#include <tuple>

class Board
{
	int* _board;
	int* _tops;
	bool _end;
	int chessCnt = std::numeric_limits<int>::max();

	bool rivalWin(Point pos) const;
	bool selfWin(Point pos) const ;
public:
	void init(const int* board, const int* tops);
	void placeChess(Point pos, Player p);
	void removeChess(Point pos);

	bool isEnd() const { return _end; }
	bool valid(int i) const { return _tops[i] > 0; }
	int getTop(int i) const { return _tops[i]; }

	bool tie() const;
	void printState() const;

	std::pair<int, int> checkBoard();

	friend class Game;

	~Board();
};
