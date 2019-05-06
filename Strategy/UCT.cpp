#include "UCT.hpp"
#include "node.hpp"
#include "judge.hpp"
#include "timer.hpp"
#include "game.hpp"

#ifndef CUSTOM_MAIN
#include "conio.h"
#endif

#include <cstring>
#include <cstdlib>
#include <tuple>

static constexpr double max_search_time = 1.5;
static constexpr double ucb_coefficient = 0.8;

// need refactor
static bool checkEnd(int* board, int* tops, Point pos, Player p) {
    if (p == Player::P_RIVAL && userWin(pos.x, pos.y, row(), column(), board)) 
        return true;
    if (p == Player::P_SELF && machineWin(pos.x, pos.y, row(), column(), board))
        return true;
    if (isTie(column(), tops))
        return true;

    return false;
}

static std::pair<int, int> checkBoard(int* board, int* tops) {
	int rival_pos = -1;
	int self_pos = -1;
	for (int i = 0; i < column(); ++i) {
		if (tops[i] > 0) {
			int newY = i;
			int newX = tops[newY] - 1;
			
			// assume place rival's chess there
			board[newX * column() + newY] = 1;
			if (userWin(newX, newY, row(), column(), board))
				rival_pos = newY;

			// assume place my chess there
			board[newX * column() + newY] = 2;
			if (machineWin(newX, newY, row(), column(), board))
				self_pos = newY;

			board[newX * column() + newY] = 0;
		}
	}

	return std::make_pair(rival_pos, self_pos);
}

UCTTree::~UCTTree() {
    if (root) {
        delete root;
    }
}

Point UCTTree::next() {

	int* originBoard = root->board;
	int* originTops = root->tops;

	auto check_retval = checkBoard(originBoard, originTops);
	if (check_retval.second >= 0) {
		int c = check_retval.second;
		return Point{ originTops[c] - 1, c };
	} else if (check_retval.first >= 0) {
		int c = check_retval.first;
		return Point{ originTops[c] - 1, c };
	}

    Timer t;
    while (t.elapsed() < max_search_time) {
        Node* selected = getTargetNode(root);
		double profit = 0.0;
		profit = simulate(selected);
        selected->update(profit);
    }

	int maxVisited = 0;
	Node* target = nullptr;

#ifndef CUSTOM_MAIN
		_cprintf("==========RESULT:=========\n");
#endif

	for (int i = 0; i < column(); ++i) {
		Node* n = root->child[i];

#ifndef CUSTOM_MAIN
		if (n)
			_cprintf("child %d, %d: %d %lf\n", n->pos.x, n->pos.y, n->cnt, n->profit);
		else
			_cprintf("child %d is null\n", i);
#endif

		if (n && (n->cnt > maxVisited)) {
			auto check_next_place = checkBoard(n->board, n->tops);
			if (check_next_place.first >= 0)
				continue;
			maxVisited = n->cnt;
			target = n;
		}
	}

	if (!target) {
		for (int i = 0; i < column(); ++i) {
			if (root->child[i]) {
				target = root->child[i];
				break;
			}
		}
	}
	return target->pos;
}

Node* UCTTree::getTargetNode(Node* base) const {
    Node* cur = base;
    while (!cur->isleaf) {
        if (cur->expandable()) {
            return cur->expand();
        } else {
            cur = cur->selectChild(ucb_coefficient);
        }
    }

    return cur;
}

double UCTTree::simulate(Node* base) const {

    int* fakeBoard = new int[row() * column() * sizeof(int)];
    int* fakeTops = new int[column() * sizeof(int)];

    std::memcpy(fakeBoard, base->board, row() * column() * sizeof(int));
    std::memcpy(fakeTops, base->tops, column() * sizeof(int));

    Point lastpos = base->pos;
    Player curP = revert(base->player);

    while (!checkEnd(fakeBoard, fakeTops, lastpos, curP)) {
		if (curP == Player::P_RIVAL) {
			auto check_place = checkBoard(fakeBoard, fakeTops);
			if (check_place.second >= 0) {
				curP = revert(curP);
				int targetC = check_place.second;
				int targetR = --fakeTops[targetC];
				if (targetR - 1 == nopos().x &&
					targetC == nopos().y)
					fakeTops[targetC]--;
				fakeBoard[targetR * column() + targetC] = toCode(curP);
				break;
			}
			if (check_place.first >= 0) {
				curP = revert(curP);
				int targetC = check_place.first;
				int targetR = --fakeTops[targetC];
				if (targetR - 1 == nopos().x &&
					targetC == nopos().y)
					fakeTops[targetC]--;
				fakeBoard[targetR * column() + targetC] = toCode(curP);
				continue;
			}
		}
        curP = revert(curP);
        int newY = rand() % column();
        while (fakeTops[newY] == 0)
            newY = rand() % column();

        int newX = --fakeTops[newY];

        fakeBoard[newX * column() + newY] = toCode(curP);
        if (newX - 1 == nopos().x &&
                newY == nopos().y)
            fakeTops[newY]--;

        lastpos = {newX, newY};
    }

	double retval;
	if (isTie(column(), fakeTops))
		retval = 0.0;
	else
		retval = (base->player == curP ? 1.0 : -1.0);

	delete []fakeBoard;
	delete []fakeTops;

	return retval;
}

bool UCTTree::advanceRoot(Point lastAction)
{
	Node* oldroot = root;
	bool retval = false;

	for (int i = 0; i < column(); ++i) {
		Node* child = root->child[i];
		if (child && child->pos == lastAction) {
			child->parent = nullptr;
			root = child;
			oldroot->child[i] = nullptr;
			retval = true;
			break;
		}
	}

	delete oldroot;
	if (!retval)
		root = nullptr;
	return retval;
}
