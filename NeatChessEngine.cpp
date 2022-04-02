#include "NeatChessEngine.h"

Move NeatChessEngine::GenerateNextBestMove()
{
	Move result;
	int valueBuffer = -99999999999;
	int bestValue = -99999999999;
	int alpha = -99999999;
	int beta = 99999999;
	list<Move> moveList = originalBoard.generateMoves();
	Board bufferBoard(originalBoard);

	result.startingSquare = 99;
	result.targetSquare = 99;


	// Error state
	if (moveList.size() == 0)
	{
		return result;
	}

	if (originalBoard.getFullMoveCount() == 50)
	{
		return result;
	}

	for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
	{
		bufferBoard.makeMove(*it);
		valueBuffer = alphaBetaMax(bufferBoard, alpha, beta, depthToSearch - 1);
		bufferBoard.unmakeMove(*it);

		if (valueBuffer > bestValue)
		{
			bestValue = valueBuffer;
			result = *it;
		}
	}

	return result;
}

float NeatChessEngine::alphaBetaSearch(Board node, int depth, float& alpha, float& beta)
{
	float valueBuffer = 0;
	float bestValue = 0;

	if (depth == 0)
	{
		return evaluate(node);
	}

	list<Move> moveList = node.generateMoves();
	if (moveList.size() == 0)
	{
		return node.isWhite() ? -100000 : 100000;
	}

	if (node.isWhite())
	{
		valueBuffer = -99999999; // Negative Infinite
		bestValue = -99999999;

		for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
		{
			node.makeMove(*it);
			valueBuffer = alphaBetaSearch(node, depth - 1, alpha, beta);
			node.unmakeMove(*it);

			if (valueBuffer > bestValue)
			{
				bestValue = valueBuffer;
			}

			alpha = (alpha > bestValue) ? alpha : bestValue;
			if (bestValue >= beta)
			{
				break;
			}
		}
		return bestValue;
	}
	else
	{
		valueBuffer = 99999999; // Infinite
		bestValue = 99999999;

		for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
		{
			node.makeMove(*it);
			valueBuffer = alphaBetaSearch(node, depth - 1, alpha, beta);
			node.unmakeMove(*it);

			if (valueBuffer < bestValue)
			{
				bestValue = valueBuffer;
			}

			beta = (beta < bestValue) ? beta : bestValue;
			if (bestValue <= alpha)
			{
				break;
			}
		}
		return bestValue;
	}
}

int NeatChessEngine::alphaBetaMax(Board node, int alpha, int beta, int depthLeft)
{
	if (depthLeft == 0)
	{
		return evaluate(node);
	}

	list<Move> moveList = node.generateMoves();
	float score = 0;
	for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
	{
		node.makeMove(*it);
		score = alphaBetaMin(node, alpha, beta, depthLeft - 1);
		node.unmakeMove(*it);
		if (score >= beta)
		{
			return beta;
		}
		if (score > alpha)
		{
			alpha = score;
		}
	}
	return alpha;
}

int NeatChessEngine::alphaBetaMin(Board node, int alpha, int beta, int depthLeft)
{
	if (depthLeft == 0)
	{
		return -evaluate(node);
	}

	list<Move> moveList = node.generateMoves();
	float score = 0;
	for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
	{
		node.makeMove(*it);
		score = alphaBetaMax(node, alpha, beta, depthLeft - 1);
		node.unmakeMove(*it);
		if (score <= alpha)
		{
			return alpha;
		}
		if (score < beta)
		{
			alpha = score;
		}
	}
	return beta;
}

float NeatChessEngine::evaluate(Board board)
{
#ifdef PYTHON_EVALUATE
	return pythonEvaluation(board);
#endif // PYTHON_EVALUATE
#ifdef STATIC_EVALUATE
	return staticEvaluation(board);
#endif // STATIC_EVALUATE

	// If for some reason neither are defined
	return 0;
}

float NeatChessEngine::pythonEvaluation(Board board)
{
	Py_Initialize();

	PyObject* pName, * pModule, * pFunc, * pArgs, * pValue;
	pName = PyUnicode_FromString((char*)"train_netwrok");
	pModule = PyImport_Import(pName);
	pFunc = PyObject_GetAttrString(pModule, (char*)"get_board_value");
	pArgs = PyTuple_Pack(1, PyUnicode_FromString(board.saveBoardMinusFEN().c_str()));
	pValue = PyObject_CallObject(pFunc, pArgs);
	string result = _PyUnicode_AsString(pValue);

	return std::stof(result);
}

float NeatChessEngine::staticEvaluation(Board board)
{
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
	float mobilityScore = mobilityWeight * evaluateMovement(board, isWhiteTurn);

	result = kingScore +
		queenScore +
		rookScore +
		bishopScore +
		knightScore +
		pawnScore;

	result *= (1 - (board.getFullMoveCount() / 50.0f));

	return result;
}

Move NeatChessEngine::getPlayerInput()
{
	Move result;
	string playerInput;
	int first = 0;
	int second = 0;

	cout << "Enter starting square: ";
	cin >> playerInput;
	first = cordToIndex(playerInput);

	cout << "Enter target square: ";
	cin >> playerInput;
	second = cordToIndex(playerInput);

	result.startingSquare = first;
	result.targetSquare = second;
	result.promotionPieceType = EMPTY;
	return result;
}

char NeatChessEngine::getPlayerPromotionInput()
{
	char promotionType;
	cout << "Enter piece to promote pawn to {n, b, r, q}: ";
	cin >> promotionType;
	return promotionType;
}

float NeatChessEngine::evaluateMovement(Board board, bool forWhite)
{
	U64 whitePieces = 0ULL;
	U64 blackPieces = 0ULL;
	float whiteBuffer = 0;
	float blackBuffer = 0;

	for (int i = WHITE_PAWN; i <= WHITE_KING; i++)
	{
		if (i == WHITE_PAWN)
		{
			whitePieces = board.getPieces(i);
			blackPieces = board.getPieces(i + BLACK_PIECES);

			for (int j = 0; j < 4; j++)
			{
				whiteBuffer += ((j + 1) * .25f) * count_1s(whitePieces & whitePawnHeatMap[j]);
				blackBuffer += ((j + 1) * .25f) * count_1s(blackPieces & blackPawnHeatMap[j]);
			}
		}
		if (i == WHITE_KNIGHT)
		{
			whitePieces = board.getPieces(i);
			blackPieces = board.getPieces(i + BLACK_PIECES);

			for (int j = 0; j < 4; j++)
			{
				whiteBuffer += ((j + 1) * .25f) * count_1s(whitePieces & knightHeatMap[j]);
				blackBuffer += ((j + 1) * .25f) * count_1s(blackPieces & knightHeatMap[j]);
			}
		}
		if (i == WHITE_BISHOP)
		{
			whitePieces = board.getPieces(i);
			blackPieces = board.getPieces(i + BLACK_PIECES);

			for (int j = 0; j < 4; j++)
			{
				whiteBuffer += ((j + 1) * .25f) * count_1s(whitePieces & bishopHeatMap[j]);
				blackBuffer += ((j + 1) * .25f) * count_1s(blackPieces & bishopHeatMap[j]);
			}
		}
		if (i == WHITE_ROOK)
		{
			whitePieces = board.getPieces(i);
			blackPieces = board.getPieces(i + BLACK_PIECES);

			for (int j = 0; j < 4; j++)
			{
				whiteBuffer += ((j + 1) * .25f) * count_1s(whitePieces & rookHeatMap[j]);
				blackBuffer += ((j + 1) * .25f) * count_1s(blackPieces & rookHeatMap[j]);
			}
		}
		if (i == WHITE_QUEEN)
		{
			whitePieces = board.getPieces(i);
			blackPieces = board.getPieces(i + BLACK_PIECES);

			for (int j = 0; j < 4; j++)
			{
				whiteBuffer += ((j + 1) * .25f) * count_1s(whitePieces & queenHeatMap[j]);
				blackBuffer += ((j + 1) * .25f) * count_1s(blackPieces & queenHeatMap[j]);
			}
		}
		if (i == WHITE_KING)
		{
			whitePieces = board.getPieces(i);
			blackPieces = board.getPieces(i + BLACK_PIECES);

			for (int j = 0; j < 4; j++)
			{
				whiteBuffer += ((j + 1) * .25f) * count_1s(whitePieces & whiteKingHeatMap[j]);
				blackBuffer += ((j + 1) * .25f) * count_1s(blackPieces & blackKingHeatMap[j]);
			}
		}
	}

	if (forWhite)
	{
		return whiteBuffer - blackBuffer;
	}
	else
	{
		return blackBuffer - whiteBuffer;
	}
}

NeatChessEngine::NeatChessEngine()
{
	depthToSearch = DEPTH;
	enginePlayingAsWhite = false;
	lastPieceTypeMoved = EMPTY;
	// Initialize Random Seed
	srand(time(NULL));
	mobilityWeight = .5f;
}

NeatChessEngine::~NeatChessEngine()
{
}

void NeatChessEngine::GameLoop(bool playerIsWhite)
{
	// If engine plays white
	if (enginePlayingAsWhite)
	{
		// TODO replace wile condition
		while (originalBoard.getMoveCount(originalBoard, 1) > 0)
		{
			// White turn
			if (originalBoard.isWhite())
			{
				try
				{
					originalBoard.makeMove(GenerateNextBestMove());
				}
				catch (std::exception e)
				{
					cout << "ERROR:" << endl;
					cout << "\tMove count: " << originalBoard.getMoveCount(originalBoard, 1);
				}
			}
			// Black turn
			else
			{
				originalBoard.displayConsole();
				originalBoard.makeMove(getPlayerInput());
				if (originalBoard.isPromotionAvaliable())
				{
					originalBoard.promotePawn(getPlayerPromotionInput());
				}
			}
		}
	}
	// If engine plays black
	else
	{
		// TODO replace while condition
		while (originalBoard.getMoveCount(originalBoard, 1) > 0)
		{
			// White turn
			if (originalBoard.isWhite())
			{
				originalBoard.displayConsole();
				originalBoard.makeMove(getPlayerInput());
				if (originalBoard.isPromotionAvaliable())
				{
					originalBoard.promotePawn(getPlayerPromotionInput());
				}
			}
			// Black turn
			else
			{
				originalBoard.makeMove(GenerateNextBestMove());
			}
		}
	}
}

void NeatChessEngine::SoloGameLoop()
{
	// TODO replace wile condition
	while (originalBoard.getMoveCount(originalBoard, 1) > 0)
	{
		if (originalBoard.isWhite())
		{
			originalBoard.displayConsole();
			originalBoard.makeMove(getPlayerInput());
			if (originalBoard.isPromotionAvaliable())
			{
				originalBoard.promotePawn(getPlayerPromotionInput());
			}
		}
		// Black turn
		else
		{
			originalBoard.displayConsole();
			originalBoard.makeMove(getPlayerInput());
			if (originalBoard.isPromotionAvaliable())
			{
				originalBoard.promotePawn(getPlayerPromotionInput());
			}
		}
	}


}

void NeatChessEngine::automatedGameLoop()
{
	int count = 0;
	Move moveBuffer;
	// TODO replace wile condition
	while (originalBoard.getMoveCount(originalBoard, 1) > 0 &&
		originalBoard.getFullMoveCount() < 50 &&
		originalBoard.getStateCount() < 3)
	{
		moveBuffer = GenerateNextBestMove();
		originalBoard.makeMove(moveBuffer);
		gameHistory.push_back(moveBuffer);
		lastPieceTypeMoved = originalBoard.getSquareType(moveBuffer.targetSquare);
	}

	//if (originalBoard.isChecked())
	//{
	//	cout << (originalBoard.isWhite() ? "Black" : "White") << " wins" << endl;
	//}
	//else if (originalBoard.getFullMoveCount() >= 50)
	//{
	//	cout << "Draw 50 step rule" << endl;
	//}
	//else if (originalBoard.getStateCount() >= 3)
	//{
	//	cout << "3-fold rule" << endl;
	//}
	//else
	//{
	//	cout << "Unexpected end? Maybe a stalemate?" << endl;
	//	cout << "Current turn is " << (originalBoard.isWhite() ? "White" : "Black") << " turn" << endl;
	//}
}

void NeatChessEngine::displayBoard()
{
	originalBoard.displayConsole();
}

void NeatChessEngine::loadFEN(string FEN)
{
	originalBoard.loadBoard(FEN);
}

string NeatChessEngine::getResult()
{
	if (originalBoard.getMoveCount(originalBoard, 1) == 0)
	{
		if (originalBoard.isChecked())
		{
			return originalBoard.isWhite() ? BLACK_WIN : WHITE_WIN;
		}
		else if (originalBoard.getStateCount() == 3)
		{
			return THREEFOLD_REPITION;
		}
		else
		{
			return STALEMATE;
		}
	}

	if (originalBoard.getFullMoveCount() == 50)
	{
		return DRAW;
	}

	return THREEFOLD_REPITION;
}

void NeatChessEngine::setPlayingWhite(bool playingWhite)
{
	enginePlayingAsWhite = playingWhite;
}

void NeatChessEngine::setDepth(int depth)
{
	depthToSearch = depth;
}

void NeatChessEngine::setMovementWeight(float movementWeight)
{
	mobilityWeight = movementWeight;
}

bool NeatChessEngine::justMovedPawn()
{
	return lastPieceTypeMoved == WHITE_PAWN || lastPieceTypeMoved == BLACK_PAWN;
}

bool NeatChessEngine::getPlayingWhite()
{
	return enginePlayingAsWhite;
}

int NeatChessEngine::getDepth()
{
	return depthToSearch;
}

list<Move> NeatChessEngine::getGameHistory()
{
	return gameHistory;
}

void NeatChessEngine::makeMove(Move move)
{
	originalBoard.makeMove(move);
}

Move NeatChessEngine::GenerateRandomMove()
{
	list<Move> moveList = originalBoard.generateMoves();

	int index = rand() % moveList.size();

	for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
	{
		if (index <= 0)
		{
			return *it;
		}

		index--;
	}
}
