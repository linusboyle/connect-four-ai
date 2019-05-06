#include "debug.hpp"
#include "conio.h"
#include "game.hpp"

void printBoard(int* board) {
	_cprintf("Print board:\n");
	for (int i = 0; i < row(); ++i) {
		for (int j = 0; j < column(); ++j) {
			_cprintf("%d ", board[i * column() + j]);
		}
		_cprintf("\n");
	}
}

void printTops(int* tops) {
	_cprintf("Print tops:\n");
	for (int i = 0; i < column(); ++i) {
		_cprintf("%d ", tops[i]);
	}
	_cprintf("\n");
}
