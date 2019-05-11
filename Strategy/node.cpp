#include "node.hpp"
#include "mempool.hpp"
#include <stdexcept>

#include <cstdlib>
#include <cmath>
#include <limits>
#include <utility>

Node::Node(Player player, Point pos, Node* parent, bool isleaf) :
	player(player), pos(pos), parent(parent),
	cnt(0), profit(0.0), expandableCnt(0),
	isleaf(isleaf) {
		Board& b = Game::instance().getBoard();
		for (int i = 0; i < column(); ++i) {
			if (b.valid(i)) {
				expandableIndex[expandableCnt++] = i;
			}
			child[i] = nullptr;
		}
	}

Node* Node::expand() {
	Board& b = Game::instance().getBoard();

    Player newPlayer = revert(player);
    int index = rand() % expandableCnt;
    int newY = expandableIndex[index];
    int newX = b.getTop(newY)-1;

	b.placeChess({ newX, newY }, player);

	child[newY] = new Node(newPlayer, { newX, newY }, this, b.isEnd());
	std::swap(expandableIndex[index], expandableIndex[--expandableCnt]);

    return child[newY];
}

Node* Node::selectChild(double coefficient) const {
    Node* retval { nullptr };

    double maxProfit = std::numeric_limits<double>::lowest();
    for (int i = 0; i < column(); ++i) {
        if (child[i]) {
            int visited = child[i]->cnt;
            double cprofit = child[i]->profit;
			double temp = - cprofit / visited + std::sqrt(2 * std::log(cnt) / visited) * coefficient;

            if (temp > maxProfit) {
                maxProfit = temp;
                retval = child[i];
            }
        }
    }

	if (!retval)
		throw std::invalid_argument("null child");
    return retval;
}

void Node::update(double delta) {
    Node* target = this;
    while (target) {
        target->cnt++;
        target->profit += delta;
        target = target->parent;
        delta = -delta;
    }
}

void* Node::operator new(std::size_t size) {
    return MemPool<Node>::instance().get();
}

void Node::operator delete(void* doomed, std::size_t size) {
    MemPool<Node>::instance().push(doomed);
}
