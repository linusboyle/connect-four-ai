#include "board.hpp"
#include "game.hpp"
#include <cstring>

#ifdef DEBUG
#include "conio.h"
#else
#include <cstdio>
#endif

void Board::init(const int* board, const int* tops) {
	if (!_tops)
		_tops = new int[max_board_width];
	if (!_board)
		_board = new int[max_board_width * max_board_width];

	std::memcpy(_tops, tops, column() * sizeof(int));
	std::memcpy(_board, board, row() * column() * sizeof(int));

	// the pointers are passed from the apis, which
	// always corresponds with a running state of
	// the game
	_end = false;

	// calculate the number of chesses on the board
	chessCnt = 0;
	for (int i = 0; i < row(); ++i) {
		for (int j = 0; j < column(); ++j) {
			if (_board[i * column() + j] > 0)
				chessCnt++;
		}
	}
}

void Board::placeChess(Point pos, Player p) {
	chessCnt++;
	_board[pos.x * column() + pos.y] = player2Code(p);
	_tops[pos.y]--;

	if (pos.x - 1 == nopos().x &&
		pos.y == nopos().y)
		_tops[pos.y]--;

	if (p == Player::P_RIVAL && rivalWin(pos))
		_end = true;
	else if (p == Player::P_SELF && selfWin(pos))
		_end = true;
	else if (tie())
		_end = true;
	else
		_end = false;
}

bool Board::rivalWin(Point pos) const {
	int i, j;
	int x = pos.x;
	int y = pos.y;

    int count = 0;
    for (i = y; i >= 0; i--)
        if (!(_board[x * column() + i] == 1))
            break;
    count += (y - i);
    for (i = y; i < column(); i++)
        if (!(_board[x * column() + i] == 1))
            break;
    count += (i - y - 1);
    if (count >= 4) return true;

    count = 0;
    for (i = x; i < row(); i++)
        if (!(_board[i * column() + y] == 1))
            break;
    count += (i - x);
    if (count >= 4) return true;

    count = 0;
    for (i = x, j = y; i < row() && j >= 0; i++, j--)
        if (!(_board[i * column() + j] == 1))
            break;
    count += (y - j);
    for (i = x, j = y; i >= 0 && j < column(); i--, j++)
        if (!(_board[i * column() + j] == 1))
            break;
    count += (j - y - 1);
    if (count >= 4) return true;

    count = 0;
    for (i = x, j = y; i >= 0 && j >= 0; i--, j--)
        if (!(_board[i * column() + j] == 1))
            break;
    count += (y - j);
    for (i = x, j = y; i < row() && j < column(); i++, j++)
        if (!(_board[i * column() + j] == 1))
            break;
    count += (j - y - 1);
    if (count >= 4) return true;

    return false;
}

bool Board::selfWin(Point pos) const {
	int x = pos.x;
	int y = pos.y;
    int i, j;

    int count = 0;
    for (i = y; i >= 0; i--)
        if (!(_board[x * column() + i] == 2))
            break;
    count += (y - i);
    for (i = y; i < column(); i++)
        if (!(_board[x * column() + i] == 2))
            break;
    count += (i - y - 1);
    if (count >= 4) return true;

    count = 0;
    for (i = x; i < row(); i++)
        if (!(_board[i * column() + y] == 2))
            break;
    count += (i - x);
    if (count >= 4) return true;

    count = 0;
    for (i = x, j = y; i < row() && j >= 0; i++, j--)
        if (!(_board[i * column() + j] == 2))
            break;
    count += (y - j);
    for (i = x, j = y; i >= 0 && j < column(); i--, j++)
        if (!(_board[i * column() + j] == 2))
            break;
    count += (j - y - 1);
    if (count >= 4) return true;

    count = 0;
    for (i = x, j = y; i >= 0 && j >= 0; i--, j--)
        if (!(_board[i * column() + j] == 2))
            break;
    count += (y - j);
    for (i = x, j = y; i < row() && j < column(); i++, j++)
        if (!(_board[i * column() + j] == 2))
            break;
    count += (j - y - 1);
    if (count >= 4) return true;

    return false;
}

bool Board::tie() const {
	bool isTie = true;
    for (int i = 0; i < column(); i++)
    {
        if (_tops[i] > 0)
        {
            isTie = false;
            break;
        }
    }
    return isTie;
}

#ifdef DEBUG
void Board::printState() const
{
	_cprintf("Print board:\n");
	for (int i = 0; i < row(); ++i) {
		for (int j = 0; j < column(); ++j) {
			_cprintf("%d ", _board[i * column() + j]);
		}
		_cprintf("\n");
	}
	_cprintf("Print tops:\n");
	for (int i = 0; i < column(); ++i) {
		_cprintf("%d ", _tops[i]);
	}
	_cprintf("\n");
}
#else
void Board::printState() const
{
	printf("Print board:\n");
	for (int i = 0; i < row(); ++i) {
		for (int j = 0; j < column(); ++j) {
			printf("%d ", _board[i * column() + j]);
		}
		printf("\n");
	}
	printf("Print tops:\n");
	for (int i = 0; i < column(); ++i) {
		printf("%d ", _tops[i]);
	}
	printf("\n");
}
#endif

Board::~Board()
{
	if (_board)
		delete[]_board;

	if (_tops)
		delete[]_tops;
}

void Board::removeChess(Point pos) {
	chessCnt--;
	_board[pos.x * column() + pos.y] = 0;
	_tops[pos.y]++;
	if (pos.x - 1 == nopos().x
		&& pos.y == nopos().y)
		_tops[pos.y]++;

	_end = false;
}

std::pair<int, int> Board::checkBoard() {
		int rival_pos = -1;
		int self_pos = -1;
		for (int i = 0; i < column(); ++i) {
			if (_tops[i] > 0) {
				int newY = i;
				int newX = _tops[newY] - 1;

				// assume place rival's chess there
				_board[newX * column() + newY] = 1;
				if (rivalWin({ newX, newY }))
					rival_pos = newY;

				// assume place my chess there
				_board[newX * column() + newY] = 2;
				if (selfWin({newX, newY }))
					self_pos = newY;
	
				_board[newX * column() + newY] = 0;
			}
		}
	
		return std::make_pair(rival_pos, self_pos);
	}