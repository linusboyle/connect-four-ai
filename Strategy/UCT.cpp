#include "UCT.hpp"
#include "node.hpp"
#include "judge.hpp"
#include "timer.hpp"

#ifndef CUSTOM_MAIN
#include "conio.h"
#endif

#include <cstring>
#include <cstdlib>
#include <tuple>
#include <cmath>

static constexpr double max_search_time = 2.0;
static constexpr double ucb_coefficient = 1.0;

// need refactor
static bool checkEnd(int* board, int* tops, Point pos, Player p) {
    if (p == Player::P_RIVAL && userWin(pos.x, pos.y, config::row, config::column, board)) 
        return true;
    if (p == Player::P_SELF && machineWin(pos.x, pos.y, config::row, config::column, board))
        return true;
    if (isTie(config::column, tops))
        return true;

    return false;
}

static std::pair<int, int> checkBoard(int* board, int* tops) {
	int rival_pos = -1;
	int self_pos = -1;
	for (uint32_t i = 0; i < config::column; ++i) {
		if (tops[i] > 0) {
			int newY = i;
			int newX = tops[newY] - 1;
			
			// assume place rival's chess there
			board[newX * config::column + newY] = 1;
			if (userWin(newX, newY, config::row, config::column, board))
				rival_pos = newY;

			// assume place my chess there
			board[newX * config::column + newY] = 2;
			if (machineWin(newX, newY, config::row, config::column, board))
				self_pos = newY;

			board[newX * config::column + newY] = 0;
		}
	}

	return std::make_pair(rival_pos, self_pos);
}

UCTTree::~UCTTree() {
    if (root) {
        delete root;
    }
}

Point UCTTree::search(int* originBoard, int* originTops, Point lastpos) {

	auto check_retval = checkBoard(originBoard, originTops);
	if (check_retval.second >= 0) {
		int column = check_retval.second;
		return Point{ originTops[column] - 1, column };
	}
	else if (check_retval.first >= 0) {
		int column = check_retval.first;
		return Point{ originTops[column] - 1, column };
	}
	 
	root = new Node {
		originBoard, originTops,
		Player::P_SELF, lastpos,
		nullptr,
		false
	};

    Timer t;
    while (t.elapsed() < max_search_time) {
        Node* selected = getTargetNode(root);
		double profit = 0.0;
		
		// for (int i = 0; i < 20; ++i) {
		profit = monteCarlo(selected);
		// }
        selected->update(profit);
		// _cprintf("selected node with pos:%d %d, profit is %lf\n", selected->pos.x, selected->pos.y, profit);
    }
	uint32_t maxVisited = 0;
	Node* target = nullptr;

#ifndef CUSTOM_MAIN
		_cprintf("==========RESULT:=========\n");
#endif

	for (uint32_t i = 0; i < config::column; ++i) {
		Node* n = root->child[i];

#ifndef CUSTOM_MAIN
		if (n)
			_cprintf("child %d, %d: %d %lf\n", n->pos.x, n->pos.y, n->cnt, n->profit);
		else
			_cprintf("child %d is null", i);
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

#ifndef CUSTOM_MAIN
		_cprintf("there is no result, debug!");
#endif

		return root->child[0]->pos;
	} else {
		return target->pos;
	}
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

double UCTTree::monteCarlo(Node* base) const {

    int* fakeBoard = new int[config::row * config::column * sizeof(int)];
    int* fakeTops = new int[config::column * sizeof(int)];

    std::memcpy(fakeBoard, base->board, config::row * config::column * sizeof(int));
    std::memcpy(fakeTops, base->tops, config::column * sizeof(int));

    Point lastpos = base->pos;
    Player curP = revert(base->player);
	// int depth = 0;

    while (!checkEnd(fakeBoard, fakeTops, lastpos, curP)) {
		// depth++;
        curP = revert(curP);
        int newY = rand() % config::column;
        while (fakeTops[newY] == 0)
            newY = rand() % config::column;

        int newX = --fakeTops[newY];

        fakeBoard[newX * config::column + newY] = toCode(curP);
        if (newX - 1 == config::nopos.x &&
                newY == config::nopos.y)
            fakeTops[newY]--;

        lastpos = {newX, newY};
    }

	double retval;
	if (isTie(config::column, fakeTops))
		retval = 0;
	else
		retval = (base->player == curP ? 1 : -1);

	delete []fakeBoard;
	delete []fakeTops;

	// return retval / std::pow(10, depth);
	return retval;
}
