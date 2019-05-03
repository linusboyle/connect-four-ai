#include "node.hpp"
#include "judge.hpp"
#include "mempool.hpp"

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <limits>
#include <utility>
#include <exception>

class unexpected_player : std::exception {
public:
    virtual const char* what() { return "invalid Player";}
};

Player revert(Player state) {
    switch (state) {
    case Player::P_SELF:
        return Player::P_RIVAL;
    case Player::P_RIVAL:
        return Player::P_SELF;
    default:
        throw new unexpected_player();
    }
}

int toCode(Player state) {
    switch (state) {
    case Player::P_SELF:
        return 2;
    case Player::P_RIVAL:
        return 1;
    default:
        throw new unexpected_player();
    }
}

Node* Node::expand() {
    Player newPlayer = revert(player);
    int index = rand() % expandableCnt;

    int* newBoard = new int[config::row * config::column];
    int* newTop = new int[config::column];
    std::memcpy(newBoard, board, config::row * config::column * sizeof(int));
    std::memcpy(newTop, tops, config::column * sizeof(int));

    int newY = expandableIndex[index];
    int newX = --newTop[newY];

    newBoard[newX * config::column + newY] = toCode(player);

    if (newX - 1 == config::nopos.x && newY == config::nopos.y) {
        newTop[newY]--;
    }

	bool isleaf = false;
    switch (player) {
    case Player::P_SELF:
        isleaf = machineWin(newX, newY, config::row, config::column, newBoard) || isTie(config::column, newTop);
		break;
    case Player::P_RIVAL:
        isleaf = userWin(newX, newY, config::row, config::column, newBoard) || isTie(config::column, newTop);
		break;
    default:
        throw new unexpected_player();
    }

    child[newY] = new Node {
        newBoard, newTop, newPlayer, 
        {newX, newY}, 
        this,
		isleaf
    };

    std::swap(expandableIndex[index], expandableIndex[--expandableCnt]);

    return child[newY];
}

Node* Node::selectChild(double coefficient) const {
    Node* retval { nullptr };

    double maxProfit = std::numeric_limits<double>::lowest();
    for (uint32_t i = 0; i < config::column; ++i) {
        if (child[i]) {
            int visited = child[i]->cnt;
            double cprofit = child[i]->profit;
            double temp = cprofit / visited + std::sqrt(2 * std::log(cnt) / visited) * coefficient;

            if (temp > maxProfit) {
                maxProfit = temp;
                retval = child[i];
            }
        }
    }

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

//void* Node::operator new(std::size_t size) {
//    return MemPool<Node>::get();
//}
//
//void Node::operator delete(void* doomed, std::size_t size) {
//    MemPool<Node>::push(doomed);
//}
