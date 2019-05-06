#include "UCT.hpp"
#include "game.hpp"
#include <cstring>

Game::Game() : 
	row(0), column(0), nopos(-1, -1), 
	turnCnt(0),
	tree(nullptr) { }

void Game::init(const int* originBoard, const int* originTops,
	int row, int column, Point nopos) {
	int* board = new int[row * column];
	int* tops = new int[column];

	std::memcpy(board, originBoard, row * column * sizeof(int));
	std::memcpy(tops, originTops, column * sizeof(int));

	this->row = row;
	this->column = column;
	this->nopos = nopos;
	turnCnt = 0;

	if (tree)
		delete tree;

	tree = new UCTTree(board, tops);
}

Point Game::getNextStep(Point rivalStep)
{
	// don't advance the tree at the first time
	if (turnCnt++ > 0)
		tree->advanceRoot(rivalStep);
	Point retval = tree->next();
	tree->advanceRoot(retval);

	return retval;
}

int row() {
	return Game::instance().row;
}

int column() {
	return Game::instance().column;
}

Point nopos() {
	return Game::instance().nopos;
}
