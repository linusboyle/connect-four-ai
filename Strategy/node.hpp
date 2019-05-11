#ifndef NODE_HPP
#define NODE_HPP

#include "game.hpp"
#include <cstddef>

struct Node {
    Player player; // who holds the chess?
    Point pos; // position of the last placed chess
    Node* parent; // parent node

    int cnt;
    double profit;
    int expandableCnt;
    int expandableIndex[max_board_width];
    Node* child[max_board_width];
	bool isleaf;
    
	Node(Player player, Point pos, Node* parent, bool isleaf);

    ~Node () {
        for (int i = 0; i < column(); ++i) {
            if (child[i]) {
                delete child[i];
            }
        }
    }

    bool expandable () const {
        return expandableCnt > 0;
    }

    Node* expand();
    Node* selectChild(double coefficient) const;
    void update(double delta);

    void* operator new(std::size_t size);
    void operator delete(void* doomed, std::size_t size);
};

#endif
