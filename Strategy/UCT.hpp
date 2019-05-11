#ifndef UCT_HPP
#define UCT_HPP

struct Node;

#include "point.hpp"

class UCTTree {
	int simulateCnt;
    Node* root;

    Node* getTargetNode(Node* base) const;
    double simulate(Node* base) const;

	void processOnce() const;
	Node* getTargetByTime() const;
	Node* getTargetByRatio() const;

#ifdef DEBUG
	void printDebugInfo() const;
#endif

public:
	UCTTree(Point lastpos);

    ~UCTTree();

	bool advanceRoot(Point lastAction);
	Point next();
};
#endif
