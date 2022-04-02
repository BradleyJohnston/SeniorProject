#ifndef NEAT_CHESS_ENGINE
#define NEAT_CHESS_ENGINE

#include "Board.h"
#include <map>
#include <time.h>
#include <Python.h>

#define PYTHON_EVALUATION
//#define STATIC_EVALUATION

#define DEPTH 3

#define BLACK_WIN "Black Wins"
#define WHITE_WIN "White Wins"
#define STALEMATE "Stalemate"
#define DRAW "Draw"
#define THREEFOLD_REPITION "Threefold Repitition"

#define KING_WEIGHT 200
#define QUEEN_WEIGHT 9
#define ROOK_WEIGHT 5
#define BISHOP_WEIGHT 3
#define KNIGHT_WEIGHT 3
#define PAWN_WEIGHT 1


class NeatChessEngine
{
private:
	
	// Piece Heat Maps
	U64 whitePawnHeatMap[4] = {
		0x0000000000423C00,
		0xFF0081C3E7DBE7FF,
		0x00003C1808000000,
		0x00FF000000000000
	};
	U64 blackPawnHeatMap[4] = {
		0x003C420000000000,
		0xFFC3420000000000,
		0x00000018183C0000,
		0x000000000000FF00
	};
	U64 knightHeatMap[4] = {
		0x66C381000081C366,
		0x183C66C3C3663C18,
		0x0000182424180000,
		0x0000001818000000
	};
	U64 bishopHeatMap[4] = {
		0x3C00818181813C00,
		0x003C99C3C3993C00,
		0x0000003C3C000000,
		0xC3C342000024C3C3
	};
	U64 rookHeatMap[4] = {
		0x6200000000000062,
		0x00FF00000000FF00,
		0x0800000000000008,
		0x1400000000000014
	};
	U64 queenHeatMap[4] = {
		0x66C381000081C366,
		0x183C66C3C3663C18,
		0x0000182424180000,
		0x0000001818000000
	};
	U64 whiteKingHeatMap[4] = {
		0x00004242663C0000,
		0x0000000060063C18,
		0x00000000000000D5,
		0x000000000000002A
	};
	U64 blackKingHeatMap[4] = {
		0x0000183C66420000,
		0x007E664200000000,
		0xDD81818100000000,
		0x2200000000000000
	};

	// General Engine Variables
	Board originalBoard; // Current board state
	// Boards since last capture to test for 3 fold repition
	std::list<Move> gameHistory;
	bool enginePlayingAsWhite;
	int depthToSearch;
	int lastPieceTypeMoved;
	float mobilityWeight;

	// Alpha Beta Searching
	Move GenerateNextBestMove();
	float alphaBetaSearch(Board node, int depth, float& alpha, float& beta);

	int alphaBetaMax(Board node, int alpha, int beta, int depthLeft);
	int alphaBetaMin(Board node, int alpha, int beta, int depthLeft);
	float evaluate(Board board);
	float pythonEvaluation(Board board);
	float staticEvaluation(Board board);

	// Monte Carlo Tree Search


	// Neat aNN variables
	Move getPlayerInput();
	char getPlayerPromotionInput();
	float evaluateMovement(Board board, bool forWhite);

public:
	// Constructors
	NeatChessEngine();
	~NeatChessEngine();

	void GameLoop(bool playerIsWhite);
	void SoloGameLoop();
	void automatedGameLoop();
	void displayBoard();
	void loadFEN(string FEN);
	string getResult();

	// Setters Getters
	void setPlayingWhite(bool playingWhite);
	void setDepth(int depth);
	void setMovementWeight(float movementWeight);

	bool justMovedPawn();
	bool getPlayingWhite();
	int getDepth();
	list<Move> getGameHistory();

	void makeMove(Move move);
	Move GenerateRandomMove();
};



#endif // !NEAT_CHESS_ENGINE
