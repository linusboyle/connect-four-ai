#pragma once

#include "point.hpp"
#include "board.hpp"

class UCTTree;

class Game
{
	Game();
	Game(const Game& game) = delete;
	Game(Game&& game) = delete;
	void operator=(const Game& game) = delete;
	void operator=(Game&& game) = delete;

	int _row;
	int _column;
	Point _nopos;

	Board baseBoard;
	Board mutableBoard;

	int turnCnt;
	UCTTree* tree;

public:
	static Game& instance() { // singleton
		static Game game;
		return game;
	}

	void init(const int* originBoard, const int* originTops, int newrow, int newcolumn, Point newnopos, Point lastpos);

	Point getNextStep(Point rivalStep);

	bool checkRestart(const int* originBoard, int baseRow, int baseColumn) {
		int newChessCnt = 0;
		for (int i = 0; i < baseRow; ++i) {
			for (int j = 0; j < baseColumn; ++j) {
				if (originBoard[i * baseColumn + j] > 0)
					newChessCnt++;
			}
		}

		return newChessCnt <= baseBoard.chessCnt;
	}

	Board& getBoard() { return mutableBoard; }
	void restoreBoard() {
		mutableBoard.init(baseBoard._board, baseBoard._tops);
	}

	int row() const {
		return _row;
	}
	int column() const {
		return _column;
	}
	Point nopos() const {
		return _nopos;
	}
};

inline int row() {
	return Game::instance().row();
}

inline int column() {
	return Game::instance().column();
}

inline Point nopos() {
	return Game::instance().nopos();
}
