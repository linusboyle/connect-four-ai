#ifndef NODE_HPP
#define NODE_HPP

#include "config.hpp"
#include <cstddef>

enum class Player {
    P_RIVAL,
    P_SELF
};

Player revert(Player state);
int toCode(Player state);

struct Node {
    int* board; // the board after previous decision;
    int* tops; // the position of column tops
    Player player; // who holds the chess?
    Point pos; // position of the last placed chess
    Node* parent; // parent node

    uint32_t cnt;
    double profit;
    uint32_t expandableCnt;
    uint32_t* expandableIndex;

    Node** child; // children

	bool isleaf;
    
    Node(int* board, int* tops, Player player, Point pos, Node* parent, bool isleaf):
        board(board), tops(tops), player(player), pos(pos), parent(parent),
        cnt(0), profit(0.0), expandableCnt(0), expandableIndex(new uint32_t[config::column]),
        child(new Node*[config::column]), isleaf(isleaf) {
            for (uint32_t i = 0; i < config::column; ++i) {
                if (tops[i] > 0) {
                    expandableIndex[expandableCnt++] = i;
                }
                child[i] = nullptr;
            }
        }

    ~Node () {
        delete []board;
        delete []tops;
        delete []expandableIndex;
        for (uint32_t i = 0; i < config::column; ++i) {
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

    // void* operator new(std::size_t size);
    // void operator delete(void* doomed, std::size_t size);
};

#endif
