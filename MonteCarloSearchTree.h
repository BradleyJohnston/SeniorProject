#ifndef MONTE_CARLO_SEARCH_TREE
#define MONTE_CARLO_SEARCH_TREE

#include "MonteCarloSearchNode.h"
#include <chrono>
#include <fstream>
#include <Windows.h>

class MonteCarloSearchTree
{
private:
	float constant;
	int rolloutDepth;
	int rolloutCount;
	float evaluation(MonteCarloSearchNode* node);
public:
	MonteCarloSearchNode* root;
	MonteCarloSearchNode* selectedNode;

	MonteCarloSearchTree(Board board);
	~MonteCarloSearchTree();

	void Selection();
	void Expansion();
	float Simulation(int depth, Board board);
	void Backpropogation(float score);
	Move generateBestMove(int milisecondsToRun);

	void setConstant(float constant);
	void setDepth(int depth);
	void setCount(int count);

	float getConstant();
	int getDepth();
	int getCount();

	void saveToFile(std::string filename);
	void saveHelper(std::ofstream& fout, MonteCarloSearchNode* node);

	void runGame(resultCount results, std::string gameCode);
};


DWORD WINAPI search(LPVOID par);

#endif // !MONTE_CARLO_SEARCH_TREE