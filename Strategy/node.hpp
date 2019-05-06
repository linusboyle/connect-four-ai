#ifndef NODE_HPP
#define NODE_HPP

#include "game.hpp"

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

    int cnt;
    double profit;
    int expandableCnt;
    int* expandableIndex;

    Node** child; // children

	bool isleaf;
    
    Node(int* board, int* tops, Player player, Point pos, Node* parent, bool isleaf):
        board(board), tops(tops), player(player), pos(pos), parent(parent),
        cnt(0), profit(0.0), expandableCnt(0), expandableIndex(new int[column()]),
        child(new Node*[column()]), isleaf(isleaf) {
            for (int i = 0; i < column(); ++i) {
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
        for (int i = 0; i < column(); ++i) {
            if (child[i]) {
                delete child[i];
            }
        }
		delete []child;
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
