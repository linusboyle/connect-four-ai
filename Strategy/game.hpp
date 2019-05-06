#pragma once

#include "point.hpp"

class UCTTree;

class Game
{
	Game();
	Game(const Game& game) = delete;
	Game(Game&& game) = delete;
	void operator=(const Game& game) = delete;
	void operator=(Game&& game) = delete;

	int row;
	int column;
	Point nopos;

	int turnCnt;

	UCTTree* tree;

public:
	static Game& instance() { // singleton
		static Game game;
		return game;
	}

	void init(const int* originBoard, const int* originTops, int row, int column, Point nopos);
	Point getNextStep(Point rivalStep);

	static bool checkRestart(const int* originBoard, int baseRow, int baseColumn) {
		int sum = 0;
		for (int i = 0; i < baseRow; ++i) {
			for (int j = 0; j < baseColumn; ++j) {
				sum += originBoard[i * baseColumn + j];
			}
		}

		return sum <= 2;
	}

	friend int row();
	friend int column();
	friend Point nopos();
};
