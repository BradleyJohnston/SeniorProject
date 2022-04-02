#include "MonteCarloSearchTree.h"

#pragma warning(suppress : 4996)

#define KING_WEIGHT 200
#define QUEEN_WEIGHT 9
#define ROOK_WEIGHT 5
#define BISHOP_WEIGHT 3
#define KNIGHT_WEIGHT 3
#define PAWN_WEIGHT 1

float MonteCarloSearchTree::evaluation(MonteCarloSearchNode* node)
{
	Board board = node->getData();
	int moveCount = board.getMoveCount(board, 1);
	if (moveCount == 0)
	{
		// Checkmate
		if (board.isChecked())
		{
			return -9999999;
		}
		// Stalemate
		else
		{
			return 0;
		}
	}

	// Three fold repitions
	if (board.getStateCount() == 3)
	{
		return 0;
	}

	// 50 move rule
	if (board.getFullMoveCount() >= 50)
	{
		return 0;
	}

	float result = 0;
	bool isWhiteTurn = board.isWhite();
	int offset = (isWhiteTurn) ? WHITE_PIECES : BLACK_PIECES;
	int notOffset = (!isWhiteTurn) ? WHITE_PIECES : BLACK_PIECES;


	float kingScore = KING_WEIGHT * (count_1s(board.getPieces(WHITE_KING + offset)) -
		count_1s(board.getPieces(WHITE_KING + notOffset)));
	float queenScore = QUEEN_WEIGHT * (count_1s(board.getPieces(WHITE_QUEEN + offset)) -
		count_1s(board.getPieces(WHITE_QUEEN + notOffset)));
	float rookScore = ROOK_WEIGHT * (count_1s(board.getPieces(WHITE_ROOK + offset)) -
		count_1s(board.getPieces(WHITE_ROOK + notOffset)));
	float bishopScore = BISHOP_WEIGHT * (count_1s(board.getPieces(WHITE_BISHOP + offset)) -
		count_1s(board.getPieces(WHITE_BISHOP + notOffset)));
	float knightScore = KNIGHT_WEIGHT * (count_1s(board.getPieces(WHITE_KNIGHT + offset)) -
		count_1s(board.getPieces(WHITE_KNIGHT + notOffset)));
	float pawnScore = PAWN_WEIGHT * (count_1s(board.getPieces(WHITE_PAWN + offset)) -
		count_1s(board.getPieces(WHITE_PAWN + notOffset)));

	result = kingScore +
		queenScore +
		rookScore +
		bishopScore +
		knightScore +
		pawnScore;

	result *= (1 - (board.getFullMoveCount() / 50.0f));

	return result;

}

MonteCarloSearchTree::MonteCarloSearchTree(Board board)
{
	this->root = new MonteCarloSearchNode(board);
	selectedNode = root;
	constant = 1;
	rolloutDepth = 0;
	rolloutCount = 0;
}

MonteCarloSearchTree::~MonteCarloSearchTree()
{
	delete root;
}

void MonteCarloSearchTree::Selection()
{
	float currentUCB = 0;
	MonteCarloSearchNode* bestNode = NULL;
	selectedNode = root;

	while (selectedNode->getChildren().size() > 0)
	{
		currentUCB = selectedNode->getUCB(constant);
		bestNode = selectedNode->getBestChild(constant);
		if (bestNode->getUCB(constant) > currentUCB)
		{
			selectedNode = bestNode;
		}
		else
		{
			break;
		}
	}

	selectedNode->visit();
}

void MonteCarloSearchTree::Expansion()
{
	if (!selectedNode->isTerminal())
	{
		selectedNode = selectedNode->getUnvisitedChild();
	}
}

float MonteCarloSearchTree::Simulation(int depth, Board board)
{
	if (depth == 0)
	{
		return evaluation(selectedNode);
	}

	std::list<Move> moveList = selectedNode->getData().generateMoves();

	if (moveList.size() == 0)
	{
		return evaluation(selectedNode);
	}

	int index = std::rand() % moveList.size();

	for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
	{
		if (index <= 0)
		{
			board.makeMove(*it);
			return Simulation(depth - 1, board);
		}
		index--;
	}
}

void MonteCarloSearchTree::Backpropogation(float score)
{
	// Update the score of every node we visited
	while (selectedNode != root)
	{
		selectedNode->addValue(score);
		selectedNode->visit();
		selectedNode = selectedNode->getParent();
	}

	// Make sure we update the parent as well
	selectedNode->addValue(score);
	selectedNode->visit();
	selectedNode = selectedNode->getParent();
}

/*

	MonteCarloSearchTree* var = ((MonteCarloSearchTree*)par);
	float score;
	while (true)
	{
		var->Selection();
		var->Expansion();
		score = 0;
		Board bufferBoard = var->selectedNode->getData();
		for (int i = 0; i < var->getCount(); i++)
		{
			score += var->Simulation(var->getDepth(), bufferBoard);
		}
	}

	return 0;

*/

Move MonteCarloSearchTree::generateBestMove(int milisecondsToRun)
{

	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	float score = 0;


	while (true)
	{
		Selection();
		Expansion();

		score = 0;

		Board bufferBoard = selectedNode->getData();

		for (int i = 0; i < getCount(); i++)
		{
			score += Simulation(getDepth(), bufferBoard);
		}
		if (std::chrono::high_resolution_clock::now() - start > std::chrono::milliseconds(milisecondsToRun))
		{
			break;
		}
	}

	MonteCarloSearchNode* result = root->getBestChild(constant);
	selectedNode = result;
	return result->getMove();
}

void MonteCarloSearchTree::setConstant(float constant)
{
	this->constant = constant;
}

void MonteCarloSearchTree::setDepth(int depth)
{
	this->rolloutDepth = depth;
}

void MonteCarloSearchTree::setCount(int count)
{
	this->rolloutCount = count;
}

float MonteCarloSearchTree::getConstant()
{
	return constant;
}

int MonteCarloSearchTree::getDepth()
{
	return rolloutDepth;
}

int MonteCarloSearchTree::getCount()
{
	return rolloutCount;
}

void MonteCarloSearchTree::saveToFile(string filename)
{
	std::ofstream fout(filename.c_str());

	selectedNode = root;
	saveHelper(fout, selectedNode);
}

void MonteCarloSearchTree::saveHelper(std::ofstream& fout, MonteCarloSearchNode* node)
{
	if (node->getVisited() == 0)
	{
		return;
	}
	fout << node->getString(constant);
	std::list<MonteCarloSearchNode*> children = node->getChildren();

	for (std::list<MonteCarloSearchNode*>::iterator it = children.begin(); it != children.end(); it++)
	{
		if (*it != NULL)
		{
			saveHelper(fout, (*it));
		}
	}
}

void MonteCarloSearchTree::runGame(resultCount results, std::string gameCode)
{
	selectedNode = root;

	int pos;
	string subString = "";
	char bufferChar[2];

	int startingSquare = 0;
	int targetSquare = 0;

	while ((pos = gameCode.find(" ")) != string::npos)
	{

		subString = gameCode.substr(0, pos);
		gameCode.erase(0, pos + 1);
		bufferChar[0] = subString[0];
		bufferChar[1] = subString[1];
		startingSquare = cordToIndex(bufferChar);

		bufferChar[0] = subString[2];
		bufferChar[1] = subString[3];
		targetSquare = cordToIndex(bufferChar);
		

		selectedNode->isTerminal();
		if (selectedNode != NULL)
		{
			selectedNode->addResult(results);
			selectedNode = selectedNode->getChild(startingSquare, targetSquare);
		}
	}

}

DWORD __stdcall search(LPVOID par)
{
	MonteCarloSearchTree* var = ((MonteCarloSearchTree*)par);
	float score;
	while (true)
	{
		var->Selection();
		var->Expansion();
		score = 0;
		Board bufferBoard = var->selectedNode->getData();
		for (int i = 0; i < var->getCount(); i++)
		{
			score += var->Simulation(var->getDepth(), bufferBoard);
		}
	}

	return 0;
}
