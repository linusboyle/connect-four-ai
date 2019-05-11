#include "node.hpp"
#include "UCT.hpp"
#include "game.hpp"
#include "timer.hpp"
#include "mempool.hpp"

#ifdef DEBUG
#include "conio.h"
#endif

#include <cstdlib>
#include <limits>
#include <stdexcept>


UCTTree::~UCTTree() {
    if (root) {
        delete root;
    }
}

UCTTree::UCTTree(Point lastpos) : simulateCnt(0) {
	root = new Node {
		Player::P_SELF,
		lastpos,
		nullptr,
		false
	};
}

Point UCTTree::next() {

	Board& b = Game::instance().getBoard();
	auto check_retval = b.checkBoard();
	if (check_retval.second >= 0) {
		int c = check_retval.second;
		return Point{ b.getTop(c) - 1, c };
	} else if (check_retval.first >= 0) {
		int c = check_retval.first;
		return Point{ b.getTop(c) - 1, c };
	}

    Timer t;
    while (t.elapsed() < max_search_time) {
		simulateCnt++;
		processOnce();
    }

	Node* basic_best = getTargetByTime();
	Node* ucb_best = getTargetByRatio();
	int max_extra_time = 20000;

	while (max_extra_time && basic_best != ucb_best) {
			max_extra_time--;
			simulateCnt++;
			processOnce();
			basic_best = getTargetByTime();
			ucb_best = getTargetByRatio();
	}

#ifdef DEBUG
	printDebugInfo();
#endif

	return basic_best->pos;
}

Node* UCTTree::getTargetNode(Node* base) const {
	// the state of mutableBoard is the same with
	// the baseBoard when a single MCTS process start
    Node* cur = base;
	Board& b = Game::instance().getBoard();
    while (!cur->isleaf) {
        if (cur->expandable()) {
			// and will be in the state of the selected
			// node when this function ends
			// this is because expand() advance the state of board
			// and we manually place chess if not expandable
            return cur->expand();
        } else {
            Node* next = cur->selectChild(ucb_coefficient);
			b.placeChess(next->pos, cur->player);
			cur = next;
        }
    }

    return cur;
}

double UCTTree::simulate(Node* base) const {
	// now when simulation starts the board
	// is assured to be in the state of base node
	Board& b = Game::instance().getBoard();

    Player curP = revert(base->player);
    while (!b.isEnd()) {
//		if (curP == Player::P_RIVAL) {
//			auto check_place = checkBoard(fakeBoard, fakeTops);
//			if (check_place.second >= 0) {
//				curP = revert(curP);
//				int targetC = check_place.second;
//				int targetR = --fakeTops[targetC];
//				if (targetR - 1 == nopos().x &&
//					targetC == nopos().y)
//					fakeTops[targetC]--;
//				fakeBoard[targetR * column() + targetC] = player2Code(curP);
//				break;
//			}
//			if (check_place.first >= 0) {
//				curP = revert(curP);
//				int targetC = check_place.first;
//				int targetR = --fakeTops[targetC];
//				if (targetR - 1 == nopos().x &&
//					targetC == nopos().y)
//					fakeTops[targetC]--;
//				fakeBoard[targetR * column() + targetC] = player2Code(curP);
//				continue;
//			}
//		}
        curP = revert(curP);
        int newY = rand() % column();
        while (!b.valid(newY))
            newY = rand() % column();

        int newX = b.getTop(newY)-1;

		b.placeChess({ newX, newY }, curP);
    }

	double retval;
	if (b.tie())
		retval = 0.0;
	else
		retval = (base->player == curP ? 1.0 : -1.0);

	return retval;
}

void UCTTree::processOnce() const
{
	Node* selected = getTargetNode(root);
	double profit = simulate(selected);
	selected->update(profit);

	// restore the board to initial state
	Game::instance().restoreBoard();
}

Node* UCTTree::getTargetByTime() const
{
	int maxVisited = 0;
	Node* retval = nullptr;
	for (int i = 0; i < column(); ++i) {
		Node* n = root->child[i];

		if (n && (n->cnt > maxVisited)) {
			maxVisited = n->cnt;
			retval = n;
		}
	}

	return retval;
}

Node* UCTTree::getTargetByRatio() const
{    
	Node* retval { nullptr };

    double maxProfit = std::numeric_limits<double>::lowest();
    for (int i = 0; i < column(); ++i) {
        if (root->child[i]) {
            int visited = root->child[i]->cnt;
            double cprofit = root->child[i]->profit;
			double temp = -cprofit / visited;

            if (temp > maxProfit) {
                maxProfit = temp;
                retval = root->child[i];
            }
        }
    }

    return retval;
}

#ifdef DEBUG
void UCTTree::printDebugInfo() const
{
	_cprintf("==========RESULT of %d simulations:=========\n", simulateCnt);

	_cprintf("Root: cnt %d, profit %lf, pos: (%d %d)\n",
		root->cnt, root->profit, root->pos.x, root->pos.y);

	for (int i = 0; i < column(); ++i) {
		Node* n = root->child[i];

		if (n)
			_cprintf("child %d, %d: %d %lf\n", n->pos.x, n->pos.y, n->cnt, n->profit);
		else
			_cprintf("child %d is null\n", i);
	}

	_cprintf("%d nodes in the mempool\n", MemPool<Node>::instance().cnt());
}
#endif // CUSTOM_MAIN

bool UCTTree::advanceRoot(Point lastAction)
{
	simulateCnt = 0;

	Node* oldroot = root;
	bool retval = false;

#ifdef DEBUG
	_cprintf("Root moved to pos (%d %d)\n", lastAction.x, lastAction.y);
#endif

	// first expand the root if expandable
	// and make sure the board is restored
	// manually
	while (oldroot->expandable()) {
		Node* newChild = oldroot->expand();
		Game::instance().getBoard()
			.removeChess(newChild->pos);
	}

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
		throw std::invalid_argument("no child");
	return retval;
}
