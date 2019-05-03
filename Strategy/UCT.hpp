#ifndef UCT_HPP
#define UCT_HPP

#include "config.hpp"

struct Node;

class UCTTree {
    Node* root;

    Node* getTargetNode(Node* base) const;
    double monteCarlo(Node* base) const;
public:
    UCTTree(int row, int column, Point nopos) : root(nullptr) {
        config::row = row;
        config::column = column;
        config::nopos = nopos;
    }

    ~UCTTree();

	Point search(int* originBoard, int* originTops, Point lastpos);
};
#endif
