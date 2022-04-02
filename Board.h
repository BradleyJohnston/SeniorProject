#pragma once
/*
* This is going to be a Piece Centric
* implementation of a chess board.
* 
* https://www.chessprogramming.org/Board_Representation
* 
* Specifically I am going to utilize the
* bitboard method mentioned specifically here:
* https://www.chessprogramming.org/Bitboards
* 
* There is the option to include a redundant
* square centric representation of the board
* called a Mailbox, however it ends up
* being less efficient in some aspects.
*/

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <list>
#include <map>

using namespace std;

/*
* Pieces Index Meaning
*
* white : 1 - 6
* black : 7 - 12
*
* any White Piece: 13
* any Black Piece: 14
*
* EMPTY: 0
*
* pawn : + 1
* knight : + 2
* bishop : + 3
* rook : + 4
* queen : + 5
* king : + 6
*/
#ifndef PIECE_CONSTANTS
#define PIECE_CONSTANTS

#ifndef EMPTY
#define EMPTY 0
#endif // Empty

#ifndef WHITE_PAWN
#define WHITE_PAWN 1
#endif // White Pawn

#ifndef WHITE_KNIGHT
#define WHITE_KNIGHT 2
#endif // White Knight

#ifndef WHITE_BISHOP 
#define WHITE_BISHOP 3
#endif // White Bishop

#ifndef WHITE_ROOK
#define WHITE_ROOK 4
#endif // White Rook

#ifndef WHITE_QUEEN
#define WHITE_QUEEN 5
#endif // White Queen

#ifndef WHITE_PIECES
#define WHITE_PIECES 0
#endif // White Pieces

#ifndef WHITE_KING
#define WHITE_KING 6
#endif // White King

#ifndef BLACK_PAWN
#define BLACK_PAWN 7
#endif // Black Pawn

#ifndef BLACK_KNIGHT
#define BLACK_KNIGHT 8
#endif // Black Knight

#ifndef BLACK_BISHOP
#define BLACK_BISHOP 9
#endif // Black Bishop

#ifndef BLACK_ROOK
#define BLACK_ROOK 10
#endif // Black Rook

#ifndef BLACK_QUEEN
#define BLACK_QUEEN 11
#endif // Black Queen

#ifndef BLACK_KING
#define BLACK_KING 12
#endif // Black King

#ifndef BLACK_PIECES
#define BLACK_PIECES 6
#endif // Black Pieces

#endif // Piece Constants

/*
* 1: white king castleing rights
* 2: white queen casteling rights
* 3: black king castleing rights
* 4: black king castleing rights
*/
#ifndef CASTLE_RIGHTS_CONSTANTS
#define CASTLE_RIGHTS_CONSTANTS

#ifndef WHITE_KING_CASTLE_RIGHTS
#define WHITE_KING_CASTLE_RIGHTS (char)0b1
#endif // White King Castle Rights

#ifndef WHITE_QUEEN_CASTLE_RIGHTS
#define WHITE_QUEEN_CASTLE_RIGHTS (char)0b10
#endif // White Queen Castle Rights

#ifndef BLACK_KING_CASTLE_RIGHTS
#define BLACK_KING_CASTLE_RIGHTS (char)0b100
#endif // Black King Castle Rights

#ifndef BLACK_QUEEN_CASTLE_RIGHTS
#define BLACK_QUEEN_CASTLE_RIGHTS (char)0b1000
#endif // Black Queen Castle Rights

#endif // Castle Rights Constants

/*
* 
* Little-Endian Rank-File Mapping
* 
* 8: 56 57 58 59 60 61 62 63
* 7: 48 49 50 51 52 53 54 55
* 6: 40 41 42 43 44 45 46 47
* 5: 32 33 34 35 36 37 38 39
* 4: 24 25 26 27 28 29 30 31
* 3: 16 17 18 19 20 21 22 23
* 2:  8  9 10 11 12 13 14 15
* 1:  0  1  2  3  4  5  6  7
*	  A  B  C  D  E  F  G  H
*/
typedef unsigned long long U64; // 64 bit data type
typedef char CastleRights;


/*
* Fancy Magic Bitboard implementation
* SOURCE: https://www.chessprogramming.org/Magic_Bitboards
*/
class SMagic {
public:
	U64* ptr;  // Pointer to attack_table
	U64 mask;  // to mask relevant squares of both lines (no outer squares)
	U64 magic; // Magic 64-bit factor
	int shift; // Shift right

	SMagic()
	{
		ptr = NULL;
		mask = 0ULL;
		magic = 0ULL;
		shift = 0;
	}

	SMagic(U64* ptr, U64 mask, U64 magic, int shift)
	{
		this->ptr = ptr;
		this->mask = mask;
		this->magic = magic;
		this->shift = shift;
	}
};

struct Move
{
	int startingSquare;
	int targetSquare;
	int previousTargetSquarePiece;
	int previousEnpassantIndex;
	int previousHalfCount;
	int previousFullCount;
	CastleRights previousCastleRights;
	bool isWhite;
	bool capturedEnpassant;
	int promotionPieceType;
	map<string, int> previousBoards;
};

class Board
{
	// memory handling variables
	bool isOriginal;

	// Board State Variables
	U64 pieces[15];
	bool isWhiteTurn;
	unsigned short enpassanteIndex;
	unsigned short halfMoveCount;
	unsigned short fullMoveCount;
	CastleRights castleRights;
	bool capturedPieceLastTurn;
	std::map<std::string, int> boardsSinceCapture;


	// Usefull Masks
	U64 notAfile = 0xFEFEFEFEFEFEFEFE;
	U64 notABfile = 0xFCFCFCFCFCFCFCFC;
	U64 notHfile = 0x7F7F7F7F7F7F7F7F;
	U64 notGHfile = 0x3F3F3F3F3F3F3F3F;
	U64 file8 = 0xff00000000000000;
	U64 file1 = 0x00000000000000ff;

	const int BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
	};

	const string attackTableFile = "attackTable.txt";
	const string magicBishopFile = "bishopMagic.txt";
	const string magicRookFile = "rookMagic.txt";
	SMagic* mBishopTbl;
	SMagic* mRookTbl;
	//int RBits[64];
	//int BBits[64];
	U64* attackTable; // array approximately 108k long

	//U64* horizontalAttackTable[64];
	//U64* diagnolAttackTable[64];

	// Magic Bitboard Methods
	void loadMagicTables();
	//void generateMagicTables();
	U64 rMask(int squareIndex);
	U64 bMask(int squareIndex);
	U64 ratt(int squareIndex, U64 block);
	U64 batt(int squareIndex, U64 block);
	U64 findMagic(int squareIndex, int bishop);

	// Usefull Tools
	U64 randomU64();
	U64 randomU64FewerBits();
	int transform(U64 var, U64 magic, int bits);
	U64 indexToU64(int index, int bits, U64 mask);
	int pop1stBit(U64* bb);

	// Move Individual Piece
	bool moveWhitePawn(int squareIndex, int targetSquareIndex);
	bool moveBlackPawn(int squareIndex, int targetSquareIndex);
	bool moveKnight(int squareIndex, int targetSquareIndex);
	bool moveBishop(int squareIndex, int targetSquareIndex);
	bool moveRook(int squareIndex, int targetSquareIndex);
	bool moveQueen(int squareIndex, int targetSquareIndex);
	bool moveKing(int squareIndex, int targetSquareIndex);
	
	// Getters
	U64 getEmpty();
	U64 getBlack();
	U64 getWhite();

	// Pawn Movement masks
	U64 soutOne(U64 b);
	U64 nortOne(U64 b);
	U64 westOne(U64 b);
	U64 eastOne(U64 b);
	U64 whiteSinglePushTargets();
	U64 whiteDoublePushTargets();
	U64 blackSinglePushTarget();
	U64 blackDoublePushTarget();
	U64 whitePawnsAbleToPush();
	U64 whitePawnsAbleToDouble();
	U64 blackPawnsAbleToPush();
	U64 blackPawnsAbleToDouble();
	U64 whitePawnTargets(int squareIndex);
	U64 blackPawnTargets(int squareIndex);

	// Pawn Attack masks
	U64 noEaOne(U64 board);
	U64 noWeOne(U64 board);
	U64 soEaOne(U64 board);
	U64 soWeOne(U64 board);
	U64 whitePawnEastAttack();
	U64 whitePawnWestAttack();
	U64 blackPawnEastAttack();
	U64 blackPawnWestAttack();

	// Knight Movement masks
	U64 noNoEa(U64 board);
	U64 noEaEa(U64 board);
	U64 soEaEa(U64 board);
	U64 soSoEa(U64 board);
	U64 noNoWe(U64 board);
	U64 noWeWe(U64 board);
	U64 soWeWe(U64 board);
	U64 soSoWe(U64 board);
	U64 getKnightAttacks(int squareIndex);

	// Fancy Magic Bitboards "Sliding Pieces"
	// https://www.chessprogramming.org/Magic_Bitboards
	U64 diagnolAttacks(U64 occ, int sqIndex);
	U64 horizontalAttacks(U64 occ, int sqIndex);

	// King movement mask
	U64 getKingAttacks(int squareIndex);

	// Square Attacked
	U64 attacksFrom(int squareIndex);
	bool isAttacked(int squareIndex, bool isPieceWhite);
	int U64toIndex(U64 board);


public:

	// Movelist generation
	list<Move> generateMoves();
	Board();
	Board(Board& rhs);
	~Board();

	// Display
	void displayConsole();

	// Load/Save
	void loadBoard(string FEN);
	string saveBoard();
	string saveBoardMinusFEN();

	// Board Interaction
	bool movePiece(int squareIndex, int targetSquareIndex, bool force = false);
	bool makeMove(Move move);
	void unmakeMove(Move move);

	// Getters
	int getMoveCount(Board node, int depth, bool displayPerft = false);
	bool isWhite()
	{
		return isWhiteTurn;
	}
	bool isPromotionAvaliable();
	void promotePawn(char promotionType);
	bool justCapturedPiece();
	U64 getPieces(int pieceType);
	bool isChecked();
	int getFullMoveCount();
	void setTurn(bool isWhite);
	int getStateCount();

	float evaluation();
	short getSquareType(int squareIndex);

	// Alpha Beta Searching
	float alphaBetaSearch(int depth, float& alpha, float& beta, bool playingForWhite);
	void GenerateBestMove(int depth = 6);

	// Testing REMOVE THIS
	U64 getPawns()
	{
		return pieces[WHITE_PAWN] | pieces[BLACK_PAWN];
	}
	U64 getKnights()
	{
		return pieces[WHITE_KNIGHT] | pieces[BLACK_KNIGHT];
	}
	U64 getBishops()
	{
		return pieces[WHITE_BISHOP] | pieces[BLACK_BISHOP];
	}
	U64 getRooks()
	{
		return pieces[WHITE_ROOK] | pieces[BLACK_ROOK];
	}
	U64 getQueens()
	{
		return pieces[WHITE_QUEEN] | pieces[BLACK_QUEEN];
	}
	U64 getKings()
	{
		return pieces[WHITE_KING] | pieces[BLACK_KING];
	}
	U64 getQueenAttacks(bool isWhite)
	{
		U64 results = 0ULL;
		U64 buffer = pieces[isWhite ? WHITE_QUEEN : BLACK_QUEEN];
		int index = 0;

		while (buffer > 0ULL)
		{
			if ((buffer & 1ULL) == 1ULL)
			{
				cerr << "Found queen on square: " << index << endl;
				results |= diagnolAttacks(~getEmpty(), index);
				results |= horizontalAttacks(~getEmpty(), index);
			}
			buffer >>= 1;
			index += 1;
		}
		return results;
	}
	U64 kingAttacks(int squareIndex) {
		return getKingAttacks(squareIndex);
	}
	unsigned char getCastlingRights() { return castleRights; }

	// Run Tests
	void testDiagnolMagics();
	void testSlidingMagics();

	//void displayDiagnolAttackTable(int squareIndex);
	//void displayHorizontalAttackTable(int squareIndex);

	//U64** getHorizontalAttackTable()
	//{
	//	return horizontalAttackTable;
	//}
	//U64** getDiagnolAttackTable()
	//{
	//	return diagnolAttackTable;
	//}
	SMagic* getMRookTable()
	{
		return mRookTbl;
	}
	SMagic* getMBishopTable()
	{
		return mBishopTbl;
	}
	//int getBBits(int index)
	//{
	//	return BBits[index];
	//}
	//int getRBits(int index)
	//{
	//	return RBits[index];
	//}


	// Operator overloading
	Board& operator =(Board& rhs);
};

// Helpfull Functions
int cordToIndex(int row, int col);
int cordToIndex(int row, char col);
int cordToIndex(string coord);
int cordToIndex(char* coord);
string indexToCoord(int squareIndex);
void displayU64(U64 var);
int count_1s(U64 var);
