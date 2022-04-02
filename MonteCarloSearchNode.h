#ifndef MONTE_CARLO_SEARCH_NODE
#define MONTE_CARLO_SEARCH_NODE

#include <list>
#include <stdlib.h>
#include <string.h>
#include "Board.h"

struct resultCount
{
	int wins;
	int draws;
	int losses;
};

class MonteCarloSearchNode
{
private:
	float value;
	resultCount resultCounter;
	Board data;
	MonteCarloSearchNode* parent;
	std::list<MonteCarloSearchNode*> children;
	int visitCount;
	void generateChildren();
	Move moveToGenerateFromParent;

public:
	MonteCarloSearchNode(Board data);
	MonteCarloSearchNode(MonteCarloSearchNode& rhs);
	~MonteCarloSearchNode();

	Board& getData();
	int getVisited();
	MonteCarloSearchNode* getParent();
	std::list<MonteCarloSearchNode*> getChildren();
	MonteCarloSearchNode* getChild(int startingSquare, int targetSquare);
	Move getMove();

	void setData(Board data);
	void setParent(MonteCarloSearchNode* node);
	void setMove(Move move);
	void visit();
	void addValue(float score);
	void addResult(resultCount result);

	float getUCB(float constant);
	bool isLeaf();
	bool isTerminal();
	MonteCarloSearchNode* getBestChild(float constant);
	MonteCarloSearchNode* getUnvisitedChild();

	string getString(float constant);
};

#endif // !MONTE_CARLO_SEARCH_NODE
