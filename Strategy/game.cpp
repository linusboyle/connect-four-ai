#include "UCT.hpp"
#include "game.hpp"

#ifdef DEBUG
#include "conio.h"
#endif

Game::Game() : 
	_row(0), _column(0), _nopos(-1, -1), 
	turnCnt(0),
	tree(nullptr), baseBoard(), mutableBoard() { }

void Game::init(const int* originBoard, const int* originTops,
	int newrow, int newcolumn, Point newnopos, Point lastpos) {

	// NOTE: must delete the tree first before the new value
	// of column is assigned,
	// because the dtor of Node depends on the old value 
	// of column!
	// this might be a drawback of the singleton used here. 
	if (tree)
		delete tree;

	// must init these before the board
	// see Board::init() for reason
	this->_row = newrow;
	this->_column = newcolumn;
	this->_nopos = newnopos;
	turnCnt = 0;

	// renew two boards
	baseBoard.init(originBoard, originTops);
	mutableBoard.init(originBoard, originTops);

	tree = new UCTTree(lastpos);
}

Point Game::getNextStep(Point rivalStep)
{
	// don't advance the tree at the first time
	if (turnCnt++ > 0) {
		// the root change of uct tree does
		// not alter the state of either board
		tree->advanceRoot(rivalStep);
		// the two boards should be the same
		// before the MCTS search
		baseBoard.placeChess(rivalStep, Player::P_RIVAL);
		mutableBoard.placeChess(rivalStep, Player::P_RIVAL);
	}
	Point retval = tree->next();

#ifdef DEBUG
	_cprintf("Turn %d Choose point (%d %d)\n", turnCnt, retval.x, retval.y);
#endif

	tree->advanceRoot(retval);
	// and should be the same 
	// after the search
	baseBoard.placeChess(retval, Player::P_SELF);
	mutableBoard.placeChess(retval, Player::P_SELF);

	return retval;
}
