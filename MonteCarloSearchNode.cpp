#include "MonteCarloSearchNode.h"

void MonteCarloSearchNode::generateChildren()
{
	std::list<Move> moveList = data.generateMoves();

	for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
	{
		Board bufferBoard(data);
		if (bufferBoard.makeMove(*it))
		{
			MonteCarloSearchNode* bufferNode = new MonteCarloSearchNode(bufferBoard);
			bufferNode->setParent(&(*this));
			bufferNode->setMove(*it);
			children.push_back(bufferNode);
			
		}
	}
}

MonteCarloSearchNode::MonteCarloSearchNode(Board data)
{
	this->data = data;
	this->parent = NULL;
	this->visitCount = 0;
	this->value = 0;
	children = std::list<MonteCarloSearchNode*>();
	this->resultCounter.wins = 0;
	this->resultCounter.draws = 0;
	this->resultCounter.losses = 0;
}

MonteCarloSearchNode::MonteCarloSearchNode(MonteCarloSearchNode& rhs)
{
	data = rhs.data;
	parent = rhs.parent;
	visitCount = rhs.visitCount;

	for (std::list<MonteCarloSearchNode*>::iterator it = rhs.children.begin(); it != rhs.children.end(); it++)
	{
		children.push_back(*it);
	}

	this->resultCounter.wins = rhs.resultCounter.wins;
	this->resultCounter.draws = rhs.resultCounter.draws;
	this->resultCounter.losses = rhs.resultCounter.losses;
}

MonteCarloSearchNode::~MonteCarloSearchNode()
{
	for (std::list<MonteCarloSearchNode*>::iterator it = children.begin(); it != children.end(); it++)
	{
		delete *it;
	}
}

Board& MonteCarloSearchNode::getData()
{
	return data;
}

int MonteCarloSearchNode::getVisited()
{
	if (this)
		return visitCount;
	else
		return 0;

}

MonteCarloSearchNode* MonteCarloSearchNode::getParent()
{
	return parent;
}

std::list<MonteCarloSearchNode*> MonteCarloSearchNode::getChildren()
{
	return children;
}

MonteCarloSearchNode* MonteCarloSearchNode::getChild(int startingSquare, int targetSquare)
{
	if (children.size() == 0)
	{
		generateChildren();
	}

	for (std::list<MonteCarloSearchNode*>::iterator it = children.begin(); it != children.end(); it++)
	{
		if ((*it)->getMove().startingSquare == startingSquare && (*it)->getMove().targetSquare == targetSquare)
		{
			return *it;
		}
	}

}

Move MonteCarloSearchNode::getMove()
{
	Move buffer;
	buffer.startingSquare = 99;
	buffer.targetSquare = 99;
	if (this)
		return moveToGenerateFromParent;
	else
	{
		return buffer;
	}
}

void MonteCarloSearchNode::setData(Board data)
{
	this->data = data;
}

void MonteCarloSearchNode::setParent(MonteCarloSearchNode* node)
{
	this->parent = node;
}

void MonteCarloSearchNode::setMove(Move move)
{
	this->moveToGenerateFromParent = move;
}

void MonteCarloSearchNode::visit()
{
	visitCount++;
}

void MonteCarloSearchNode::addValue(float score)
{
	value += score;
}

void MonteCarloSearchNode::addResult(resultCount result)
{

	if (this != NULL)
	{
		visit();
		resultCounter.wins += result.wins;
		resultCounter.draws += result.draws;
		resultCounter.losses += result.losses;
	}
}

float MonteCarloSearchNode::getUCB(float constant)
{
	float resultValue = (resultCounter.wins) - resultCounter.losses;
	return resultValue + (constant * std::sqrt(std::log((float)(parent->getVisited() != 0 ? parent->getVisited() : 1)) / (float) (visitCount != 0 ? visitCount : 1)));
}

bool MonteCarloSearchNode::isLeaf()
{
	for (std::list<MonteCarloSearchNode*>::iterator it = children.begin(); it != children.end(); it++)
	{
		if ((*it)->getVisited() == 0)
		{
			return true;
		}
	}

	return false;
}

bool MonteCarloSearchNode::isTerminal()
{
	if (data.getMoveCount(data, 1) == 0)
	{
		return true;
	}

	if (children.size() == 0)
	{
		generateChildren();
	}
	return false;
}

MonteCarloSearchNode* MonteCarloSearchNode::getBestChild(float constant)
{
	bool isWhite = data.isWhite();
	float buffer = -99999999 * isWhite ? 1 : -1;
	float bestUCB = -99999999 * isWhite ? 1 : -1;
	MonteCarloSearchNode* bestNode = NULL;

	if (isWhite)
	{
		for (std::list<MonteCarloSearchNode*>::iterator it = children.begin(); it != children.end(); it++)
		{
			buffer = (*it)->getUCB(constant);

			if (buffer > bestUCB)
			{
				bestUCB = buffer;
				bestNode = *it;
			}
		}
	}
	else
	{
		for (std::list<MonteCarloSearchNode*>::iterator it = children.begin(); it != children.end(); it++)
		{
			buffer = (*it)->getUCB(constant);

			if (buffer < bestUCB)
			{
				bestUCB = buffer;
				bestNode = *it;
			}
		}
	}
	return bestNode;
}

MonteCarloSearchNode* MonteCarloSearchNode::getUnvisitedChild()
{
	int index = std::rand() % children.size();
	for (std::list<MonteCarloSearchNode*>::iterator it = children.begin(); it != children.end(); it++)
	{
		if (index <= 0)
		{
			return *it;
		}
		index--;
	}
}

string MonteCarloSearchNode::getString(float constant)
{
	string result = "";

	result += data.saveBoardMinusFEN();
	// Board State
	result += "\n";
	// Results
	result += std::to_string(getUCB(constant)) + "\n";

	return result;
}
