#ifndef UCT_HPP
#define UCT_HPP

#include "node.hpp"

class UCTTree {
    Node* root;

    Node* getTargetNode(Node* base) const;
    double simulate(Node* base) const;

public:
	UCTTree(int* initBoard, int* initTops) : 
		root(new Node{ initBoard, initTops, Player::P_SELF, {-1, -1}, nullptr, false })
	{
	}

    ~UCTTree();

	bool advanceRoot(Point lastAction);
	Point next();
};
#endif
