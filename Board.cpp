#include "Board.h"
/*
* Method: <method_name>
* Author: Bradley Johnston
*
* Description:
*	<DESCRIPTION>
*/
void Board::loadMagicTables()
{
	// Load Attack Table
	ifstream fin(attackTableFile);
	U64 U64buffer = 0;
	int indexBuffer = 0;
	U64 maskBuffer = 0ULL;
	U64 magicBuffer = 0ULL;
	int shiftBuffer = 0;
	int index = 0;

	fin >> U64buffer;
	attackTable = new U64[U64buffer];

	while (fin >> U64buffer)
	{
		attackTable[index] = U64buffer;
		index += 1;
	}

	fin.close();

	fin.open(magicBishopFile);
	index = 0;

	while (fin >> indexBuffer >> maskBuffer >> magicBuffer >> shiftBuffer)
	{
		mBishopTbl[index] = SMagic(&attackTable[indexBuffer], maskBuffer, magicBuffer, shiftBuffer);
		index += 1;
	}
	fin.close();

	fin.open(magicRookFile);
	index = 0;
	while (fin >> indexBuffer >> maskBuffer >> magicBuffer >> shiftBuffer)
	{
		mRookTbl[index] = SMagic(&attackTable[indexBuffer], maskBuffer, magicBuffer, shiftBuffer);
		index += 1;
	}
	fin.close();

}

/**********************************
* MAGIC BITBOARD METHODS
**********************************/
/*
* Method: GENERATE_MAGIC_TABLES
* Author: Bradley Johnston
*
* Description:
*	Generate Magic Tables for use in sliding piece move generation
*/
//void Board::generateMagicTables()
//{
//
//
//	for (int i = 0; i < 64; i++)
//	{
//		RBits[i] = 0;
//		mRookTbl[i].magic = findMagic(i,  0);
//		mRookTbl[i].mask = rMask(i);
//		mRookTbl[i].shift = 64 - RBits[i];
//		mRookTbl[i].ptr = horizontalAttackTable[i];
//		cerr << "Rook: " << i << " with " << RBits[i] << " bits." << endl;
//	}
//	for (int i = 0; i < 64; i++)
//	{
//		BBits[i] = 0;
//		mBishopTbl[i].magic = findMagic(i, 1);
//		mBishopTbl[i].mask = bMask(i);
//		mBishopTbl[i].shift = 64 - BBits[i];
//		mBishopTbl[i].ptr = diagnolAttackTable[i];
//		cerr << "Bishop: " << i << " with " << BBits[i] << " bits." << endl;
//	}
//}

/*
* Method: R_MASK
* Author: Bradley Johnston
*
* Description:
*	Generate rook mask for magic bitboard sliding piece move generation.
*/
U64 Board::rMask(int squareIndex)
{
	U64 result = 0ULL;
	int rk = squareIndex / 8, fl = squareIndex % 8, r, f;
	for (r = rk + 1; r <= 6; r++)
	{
		result |= (1ULL << (fl + r * 8));
	}
	for (r = rk - 1; r >= 1; r--)
	{
		result |= (1ULL << (fl + r * 8));
	}
	for (f = fl + 1; f <= 6; f++)
	{
		result |= (1ULL << (f + rk * 8));
	}
	for (f = fl - 1; f >= 1; f--)
	{
		result |= (1ULL << (f + rk * 8));
	}
	return result;
}

/*
* Method: B_MASK
* Author: Bradley Johnston
*
* Description:
*	Generate bishop masks for sliding piece move generation
*/
U64 Board::bMask(int squareIndex)
{
	U64 result = 0ULL;
	int rk = squareIndex / 8, fl = squareIndex % 8, r, f;
	for (r = rk + 1, f = fl + 1; r <= 6 && f <= 6; r++, f++)
	{
		result |= (1ULL << (f + r * 8));
	}
	for (r = rk + 1, f = fl - 1; r <= 6 && f >= 1; r++, f--)
	{
		result |= (1ULL << (f + r * 8));
	}
	for (r = rk - 1, f = fl + 1; r >= 1 && f <= 6; r--, f++)
	{
		result |= (1ULL << (f + r * 8));
	}
	for (r = rk - 1, f = fl - 1; r >= 1 && f >= 1; r--, f--)
	{
		result |= (1ULL << (f + r * 8));
	}
	return result;
}

/*
* Method: R_ATT
* Author: Bradley Johnston
*
* Description:
*	Rook attack targets
*/
U64 Board::ratt(int squareIndex, U64 block)
{
	U64 North = 0ULL,
		East = 0ULL,
		South = 0ULL,
		West = 0ULL;

	int count = -1;
	int rk = squareIndex / 8, fl = squareIndex % 8;
	int rn, rs, fe, fw;

	// North
	for (rn = rk; rn <= 7; rn++)
	{
		if (rn != rk)
		{
			North |= (1ULL << (fl + rn * 8));
			if (block & (1ULL << (fl + rn * 8)))
				break;
		}
		East = 0ULL;
		// East
		for (fe = fl; fe <= 7; fe++)
		{
			if (fe != fl)
			{
				East |= (1ULL << (fe + rk * 8));
				if (block & (1ULL << (fe + rk * 8)))
					break;
			}
			South = 0ULL;
			// South
			for (rs = rk; rs >= 0; rs--)
			{
				if (rs != rk)
				{
					South |= (1ULL << (fl + rs * 8));
					if (block & (1ULL << (fl + rs * 8)))
						break;
				}
				West = 0ULL;
				// West
				for (fw = fl; fw >= 0; fw--)
				{
					if (fw != fl)
					{
						West |= (1ULL << (fw + rk * 8));
						if (block & (1ULL << (fw + rk * 8)))
							break;
					}
				}
			}
		}
	}
	return North | East | South | West;

}

/*
* Method: B_ATT
* Author: Bradley Johnston
*
* Description:
*	Bishop attacks
*/
U64 Board::batt(int squareIndex, U64 block)
{
	U64 NE = 0ULL,
		NW = 0ULL,
		SE = 0ULL,
		SW = 0ULL;

	int rk = squareIndex / 8, fl = squareIndex % 8;
	int r1, r2, r3, r4, f1, f2, f3, f4;

	// NE
	for (r1 = rk, f1 = fl; r1 <= 7 && f1 <= 7; r1++, f1++)
	{
		if (r1 != rk && f1 != fl)
		{
			NE |= (1ULL << (f1 + r1 * 8));
			if (block & (1ULL << (f1 + r1 * 8)))
				break;
		}
		NW = 0ULL;
		// NW
		for (r2 = rk, f2 = fl; r2 <= 7 && f2 >= 0; r2++, f2--)
		{
			if (r2 != rk && f2 != fl)
			{
				NW |= (1ULL << (f2 + r2 * 8));
				if (block & (1ULL << (f2 + r2 * 8)))
					break;
			}
			SE = 0ULL;
			// SE
			for (r3 = rk, f3 = fl; r3 >= 0 && f3 <= 7; r3--, f3++)
			{
				if (r3 != rk && f3 != fl)
				{
					SE |= (1ULL << (f3 + r3 * 8));
					if (block & (1ULL << (f3 + r3 * 8)))
						break;
				}
				SW = 0ULL;
				// SW
				for (r4 = rk, f4 = fl; r4 >= 0 && f4 >= 0; r4--, f4--)
				{
					if (r4 != rk && f4 != fl)
					{
						SW |= (1ULL << (f4 + r4 * 8));
						if (block & (1ULL << (f4 + r4 * 8)))
							break;
					}
				}
			}
		}
	}

	return NE | NW | SE | SW;

}

/*
* Method: FIND_MAGIC
* Author: Bradley Johnston
*
* Description:
*	Generate a magic for one specific instance of a magic bitboard
* 
* TODO:
* Optimize memory
*/
U64 Board::findMagic(int squareIndex, int bishop)
{
	//U64 mask, magic;
	//U64* a = new U64[4096];
	//U64* b = new U64[4096];
	//U64* used = new U64[4096];
	//U64* ptr;
	//int i, j, k, n, fail;
	//mask = bishop ? bMask(squareIndex) : rMask(squareIndex);
	//n = count_1s(mask);

	//if (bishop)
	//{
	//	//diagnolAttackTable[squareIndex] = new U64[1 << n];
	//	//
	//	//for (int count = 0; count < (1 << n); count++)
	//	//{
	//	//	diagnolAttackTable[squareIndex][count] = 0ULL;
	//	//}
	//	//
	//	//ptr = diagnolAttackTable[squareIndex];
	//	BBits[squareIndex] = n;
	//}
	//else
	//{
	//	
	//	//horizontalAttackTable[squareIndex] = new U64[1 << n];
	//	//
	//	//for (int count = 0; count < (1 << n); count++)
	//	//{
	//	//	horizontalAttackTable[squareIndex][count] = 0ULL;
	//	//}

	//	//ptr = horizontalAttackTable[squareIndex];
	//	RBits[squareIndex] = n;
	//}
	//for (i = 0; i < (1 << n); i++)
	//{
	//	b[i] = indexToU64(i, n, mask);
	//	a[i] = bishop ? batt(squareIndex, b[i]) : ratt(squareIndex, b[i]);
	//	//ptr[i] = a[i];
	//}
	//for (k = 0; k < 10000000; k++)
	//{
	//	magic = randomU64FewerBits();
	//	if (count_1s((mask * magic) & 0xFF00000000000000ULL) < 6)
	//	{
	//		continue;
	//	}

	//	for (i = 0; i < 4096; i++)
	//	{
	//		used[i] = 0ULL;
	//	}
	//	for (i = 0, fail = 0; !fail && i < (1 << n); i++)
	//	{
	//		j = transform(b[i], magic, n);
	//		if (used[j] == 0ULL)
	//		{
	//			used[j] = ptr[i];
	//		}
	//		else if (used[j] != ptr[i])
	//		{
	//			fail = 1;
	//		}
	//	}
	//	if (!fail)
	//	{
	//		return magic;
	//	}
	//}
	//cerr << "Failed to find a magic" << endl;
	return 0ULL;
}

/**********************************
* USEFULL TOOLS METHODS
**********************************/
/*
* Method: RANDOM_64
* Author: Bradley Johnston
*
* Description:
*	Generate random 64 bit variable
*/
U64 Board::randomU64()
{
	U64 u1, u2, u3, u4;
	u1 = (U64)(rand()) & 0xFFFF;
	u2 = (U64)(rand()) & 0xFFFF;
	u3 = (U64)(rand()) & 0xFFFF;
	u4 = (U64)(rand()) & 0xFFFF;
	return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

/*
* Method: RANDOM_64_FEWER_BITS
* Author: Bradley Johnston
*
* Description:
*	Generate a reduced bitcount 64 bit variable
*/
U64 Board::randomU64FewerBits()
{
	return randomU64() & randomU64() & randomU64();
}

int Board::transform(U64 var, U64 magic, int bits)
{
	return (int)((var * magic) >> (64 - bits));
}

bool Board::isPromotionAvaliable()
{
	if (!isWhiteTurn)
	{

		return pieces[WHITE_PAWN] & file8;
	}
	else
	{

		return pieces[BLACK_PAWN] & file1;
	}
}

void Board::promotePawn(char promotionType)
{
	U64 pawnToPromote;
	int index = -1;


	if (!isWhiteTurn)
	{

		pawnToPromote = pieces[WHITE_PAWN] & file8;
	}
	else
	{

		pawnToPromote = pieces[BLACK_PAWN] & file1;
	}

	while (pawnToPromote != 0ULL)
	{
		index += 1;
		pawnToPromote >>= 1;
	}

	if (promotionType == 'n')
	{
		pieces[WHITE_KNIGHT + (!isWhiteTurn ? WHITE_PIECES : BLACK_PIECES)] |= (1ULL << index);
	}
	if (promotionType == 'b')
	{
		pieces[WHITE_BISHOP + (!isWhiteTurn ? WHITE_PIECES : BLACK_PIECES)] |= (1ULL << index);
	}
	if (promotionType == 'r')
	{
		pieces[WHITE_ROOK + (!isWhiteTurn ? WHITE_PIECES : BLACK_PIECES)] |= (1ULL << index);
	}
	if (promotionType == 'q')
	{
		pieces[WHITE_QUEEN + (!isWhiteTurn ? WHITE_PIECES : BLACK_PIECES)] |= (1ULL << index);
	}

	pieces[WHITE_PAWN + (!isWhiteTurn ? WHITE_PIECES : BLACK_PIECES)] &= ~(1ULL << index);
}

bool Board::justCapturedPiece()
{
	return capturedPieceLastTurn;
}

U64 Board::getPieces(int pieceType)
{
	return pieces[pieceType];
}

bool Board::isChecked()
{
	int squareIndex = U64toIndex(isWhiteTurn ? pieces[WHITE_KING] : pieces[BLACK_KING]);
	return isAttacked(squareIndex, isWhiteTurn);
}

int Board::getFullMoveCount()
{
	return fullMoveCount;
}

void Board::setTurn(bool isWhite)
{
	isWhiteTurn = isWhite;
}

int Board::getStateCount()
{
	return boardsSinceCapture[saveBoardMinusFEN()];
}

U64 Board::indexToU64(int index, int bits, U64 mask)
{
	int i, j;
	U64 result = 0ULL;

	for (i = 0; i < bits; i++)
	{
		j = pop1stBit(&mask);
		if (index & (1 << i))
		{
			result |= (1ULL << j);
		}
	}
	return result;
}

int Board::pop1stBit(U64* bb)
{
	U64 b = *bb ^ (*bb - 1);
	unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
	*bb &= (*bb - 1);
	return BitTable[(fold * 0x783a9b23) >> 26];
}

/**********************************
* INDIVIDUAL PIECE MOVEMENT METHODS
**********************************/
/*
* Method: MOVE_WHITE_PAWN
* Author: Bradley Johnston
*
* Description:
*	wrapper to connect various white pawn movement methods
*/
bool Board::moveWhitePawn(int squareIndex, int targetSquareIndex)
{
	U64 mask = (1ULL) << squareIndex;
	U64 targetMask = (1ULL) << targetSquareIndex;
	int difference = targetSquareIndex - squareIndex;

	// If target is in single push
	if (difference == 8)
	{
		if (targetMask & whiteSinglePushTargets())
		{
			return true;
		}
	}

	// If target is in double push
	if (difference == 16)
	{
		if (targetMask & whiteDoublePushTargets())
		{
			return true;
		}
	}

	// If target is in attack
	// West
	if (difference == 7)
	{
		if (noWeOne(mask) & targetMask && ((targetMask & getBlack()) || targetSquareIndex == enpassanteIndex))
		{
			return true;
		}
	}

	// East
	if (difference == 9)
	{
		if (noEaOne(mask) & targetMask && ((targetMask & getBlack()) || targetSquareIndex == enpassanteIndex))
		{
			return true;
		}
	}
	return false;
}

/*
* Method: MOVE_BLACK_PAWN
* Author: Bradley Johnston
*
* Description:
*	wrapper to connect multiple black methods
*/
bool Board::moveBlackPawn(int squareIndex, int targetSquareIndex)
{
	U64 mask = (1ULL) << squareIndex;
	U64 targetMask = (1ULL) << targetSquareIndex;
	int difference = targetSquareIndex - squareIndex;

	// If target is in single push
	if (difference == -8)
	{
		if (targetMask & blackSinglePushTarget())
		{
			return true;
		}
	}

	// If target is in double push
	if (difference == -16)
	{
		if (targetMask & blackDoublePushTarget())
		{
			return true;
		}
	}

	// If target is in attack
	// West
	if (difference == -9)
	{
		if (soWeOne(mask) & targetMask && ((targetMask & getWhite()) || targetSquareIndex == enpassanteIndex))
		{
			return true;
		}
	}

	// East
	if (difference == -7)
	{
		if (soEaOne(mask) & targetMask && ((targetMask & getWhite()) || targetSquareIndex == enpassanteIndex))
		{
			return true;
		}
	}
	return false;
}

/*
* Method: MOVE_KNIGHT
* Author: Bradley Johnston
*
* Description:
*	wrapper for multiple knight methods
*/
bool Board::moveKnight(int squareIndex, int targetSquareIndex)
{
	int difference = targetSquareIndex - squareIndex;
	int type = getSquareType(squareIndex);

	U64 mask = (1ULL) << squareIndex;
	U64 targetMask = (1ULL) << targetSquareIndex;

	//noNoWe
	if (difference == 15)
	{
		if (targetMask & noNoWe(pieces[type] & mask))
		{
			return true;
		}
	}
	// noNoEa
	else if (difference == 17)
	{
		if (targetMask & noNoEa(pieces[type] & mask))
		{
			return true;
		}
	}
	// noWeWe
	else if (difference == 6)
	{
		if (targetMask & noWeWe(pieces[type] & mask))
		{
			return true;
		}
	}
	// noEaEa
	else if (difference == 10)
	{
		if (targetMask & noEaEa((pieces[type]) & mask))
		{
			return true;
		}
	}
	// soWeWe
	else if (difference == -10)
	{
		if (targetMask & soWeWe((pieces[type]) & mask))
		{
			return true;
		}
	}
	// soEaEa
	else if (difference == -6)
	{
		if (targetMask & soEaEa((pieces[type]) & mask))
		{
			return true;
		}
	}
	// soSoWe
	else if (difference == -17)
	{
		if (targetMask & soSoWe((pieces[type]) & mask))
		{
			return true;
		}
	}
	// soSoEa
	else if (difference == -15)
	{
		if (targetMask & soSoEa((pieces[type]) & mask))
		{
			return true;
		}
	}

	return false;
}

/*
* Method: MOVE_BISHOP
* Author: Bradley Johnston
*
* Description:
*	wrapper to move a single bishop from one square to the next
*/
bool Board::moveBishop(int squareIndex, int targetSquareIndex)
{
	U64 mask = (1ULL) << squareIndex;
	U64 targetMask = (1ULL) << targetSquareIndex;
	int pieceType = getSquareType(squareIndex);

	//cerr << "Bishop: " << squareIndex << endl;
	//displayU64(diagnolAttacks(~getEmpty(), squareIndex));

	if (targetMask & diagnolAttacks(~getEmpty(), squareIndex))
	{
		return true;
	}

	return false;
}

/*
* Method: MOVE_ROOK
* Author: Bradley Johnston
*
* Description:
*	wrapper to move a rook from first variable index to the second
*/
bool Board::moveRook(int squareIndex, int targetSquareIndex)
{
	U64 mask = (1ULL) << squareIndex;
	U64 targetMask = (1ULL) << targetSquareIndex;
	int pieceType = getSquareType(squareIndex);

	//cerr << "Rook: " << squareIndex << endl;
	//displayU64(horizontalAttacks(~getEmpty(), squareIndex));

	if (horizontalAttacks(~getEmpty(), squareIndex) & targetMask)
	{
		return true;
	}

	return false;
}

/*
* Method: MOVE_QUEEN
* Author: Bradley Johnston
*
* Description:
*	connects rook and bishop movement as necessary.
*/
bool Board::moveQueen(int squareIndex, int targetSquareIndex)
{
	U64 mask = (1ULL) << squareIndex;
	U64 targetMask = (1ULL) << targetSquareIndex;
	int pieceType = getSquareType(squareIndex);

	if ((horizontalAttacks(~getEmpty(), squareIndex) & targetMask) ||
		(diagnolAttacks(~getEmpty(), squareIndex) & targetMask))
	{
		return true;
	}

	return false;
}

/*
* Method: MOVE_KING
* Author: Bradley Johnston
*
* Description:
*	Move king from first parameter to second. Including castling.
*
* TODO
*	Remove castling rights in appropriate situations
*/
bool Board::moveKing(int squareIndex, int targetSquareIndex)
{
	U64 targetMask = 1ULL << targetSquareIndex;
	return getKingAttacks(squareIndex) & targetMask;

	//int rowDifference = (squareIndex / 8) - (targetSquareIndex / 8);
	//int colDifference = (squareIndex % 8) - (targetSquareIndex % 8);
	//int pieceType = getSquareType(squareIndex);
	//bool isWhite = pieceType == WHITE_KING;

	//U64 mask = 1ULL << squareIndex;
	//U64 targetMask = 1ULL << targetSquareIndex;

	//// Test normal moves
	//if (rowDifference >= -1 &&
	//	rowDifference <= 1 &&
	//	colDifference >= -1 &&
	//	colDifference <= 1)
	//{
	//	return true;
	//}
	//else // test castling moves
	//{
	//	U64 empty = getEmpty();

	//	// King side castling
	//	if (colDifference == -2 &&
	//		rowDifference == 0)
	//	{
	//		// Test if this move is valid
	//		

	//		if (empty & (mask << 1) &&
	//			empty & (mask << 2) &&
	//			!isAttacked(squareIndex + 1, isWhite) &&
	//			!isAttacked(squareIndex + 2, isWhite) &&
	//			((isWhite && (castleRights & WHITE_KING_CASTLE_RIGHTS)) ||
	//				(!isWhite && (castleRights & BLACK_KING_CASTLE_RIGHTS))))
	//		{
	//			pieces[pieceType - (WHITE_KING - WHITE_ROOK)] &= ~(targetMask << 1);
	//			pieces[pieceType - (WHITE_KING - WHITE_ROOK)] |= (targetMask >> 1);
	//			return true;
	//		}
	//	}
	//	// Queen side castling
	//	else if (colDifference == 2 &&
	//		rowDifference == 0)
	//	{

	//		// test if this move is valid
	//		
	//		if (empty & (mask >> 1) &&
	//			empty & (mask >> 2) &&
	//			!isAttacked(squareIndex - 1, isWhite) &&
	//			!isAttacked(squareIndex - 2, isWhite) &&
	//			((isWhite && (castleRights & WHITE_QUEEN_CASTLE_RIGHTS)) ||
	//				(!isWhite && (castleRights & BLACK_QUEEN_CASTLE_RIGHTS))))
	//		{
	//			pieces[pieceType - (WHITE_KING - WHITE_ROOK)] &= ~(targetMask >> 2);
	//			pieces[pieceType - (WHITE_KING - WHITE_ROOK)] |= targetMask << 1;
	//			return true;
	//		}
	//	}
	//}

	//return false;
}

/**********************************
* GETTERS
**********************************/
/*
* Method: GET_EMPTY
* Author: Bradley Johnston
*
* Description:
*	Returns a board containing the empty squares
*/
U64 Board::getEmpty()
{
	U64 result = ~(0ULL);
	for (int i = 1; i < 13; i++)
	{
		result ^= pieces[i];
	}

	return result;
}

U64 Board::getBlack()
{
	U64 black = 0ULL;

	for (int i = BLACK_PIECES + WHITE_PAWN; i <= BLACK_KING; i++)
	{
		black |= pieces[i];
	}
	return black;
}

U64 Board::getWhite()
{
	U64 white = 0ULL;

	for (int i = WHITE_PAWN; i <= WHITE_KING; i++)
	{
		white |= pieces[i];
	}
	return white;
}

/*
* Method: GET_SQUARE_TYPE
* Author: Bradley Johnston
*
* Description:
*	Given a board square return the piece occupying that square.
*/
short Board::getSquareType(int squareIndex)
{
	U64 mask = (1ULL) << squareIndex;

	for (int i = 1; i < 13; i++)
	{
		if ((pieces[i] & mask) > 0ULL)
		{
			return i;
		}
	}

	return 0;
}


/**********************************
* PAWN MOVEMENT MASK METHODS
**********************************/
/*
* Method: SOUT_ONE
* Author: Bradley Johnston
*
* Description:
*	returns the board state shifted down one row.
*/
U64 Board::soutOne(U64 b)
{
	return b >> 8;
}

/*
* Method: NORT_ONE
* Author: Bradley Johnston
*
* Description:
*	Returns the board shifted up one row
*/
U64 Board::nortOne(U64 b)
{
	return b << 8;
}

U64 Board::westOne(U64 b)
{
	return (b & notAfile) >> 1;
}

U64 Board::eastOne(U64 b)
{
	return (b & notHfile) << 1;
}

/*
* Method: WHITE_SINGLE_PUSH_TARGETS
* Author: Bradley Johnston
*
* Description:
*	Return the possible squares for the white squares to attack.
*/
U64 Board::whiteSinglePushTargets()
{
	return nortOne(pieces[WHITE_PAWN]) & getEmpty();
}

/*
* Method: WHITE_DOUBLE_PUSH_TARGETS
* Author: Bradley Johnston
*
* Description:
*	Returns the possible targets of white pawn double pushes.
*/
U64 Board::whiteDoublePushTargets()
{
	const U64 rank4 = U64(0x00000000FF000000);
	U64 singlePushs = whiteSinglePushTargets();
	return nortOne(singlePushs) & getEmpty() & rank4;
}

/*
* Method: BLACK_SINGLE_PUSH_TARGET
* Author: Bradley Johnston
*
* Description:
*	Return the possible targets for the black pawns to single push
*/
U64 Board::blackSinglePushTarget()
{
	return soutOne(pieces[BLACK_PAWN]) & getEmpty();
}

/*
* Method: BLACK_DOUBLE_PUSH_TARGETS
* Author: Bradley Johnston
*
* Description:
*	Returns the targets of Black pawn double pushes.
*/
U64 Board::blackDoublePushTarget()
{
	const U64 rank5 = U64(0x000000ff00000000);
	U64 singlePushs = blackSinglePushTarget();
	return soutOne(singlePushs) & getEmpty() & rank5;
}

/*
* Method: WHITE_PAWNS_ABLE_TO_PUSH
* Author: Bradley Johnston
*
* Description:
*	Returns the
*/
U64 Board::whitePawnsAbleToPush()
{
	return soutOne(getEmpty()) & pieces[WHITE_PAWN];
}

/*
* Method: WHITE_PAWNS_ABLE_TO_PUSH
* Author: Bradley Johnston
*
* Description:
*	Return the white pawns still able to double push.
*/
U64 Board::whitePawnsAbleToDouble()
{
	U64 empty = getEmpty();
	const U64 rank4 = U64(0x00000000FF000000);
	U64 emptyRank3 = soutOne(empty & rank4) & empty;
	return soutOne(emptyRank3) & pieces[WHITE_PAWN];
}

/*
* Method: BLACK_PAWNS_ABLE_TO_DOUBLE_PUSH
* Author: Bradley Johnston
*
* Description:
*	<DESCRIPTION>
*/
U64 Board::blackPawnsAbleToPush()
{
	return nortOne(getEmpty()) & pieces[BLACK_PAWN];
}

/*
* Method: BLACK_PAWNS_ABLE_TO_DOUBLE_PUSH
* Author: Bradley Johnston
*
* Description:
*	Returns the black pawns able to push two rows
*/
U64 Board::blackPawnsAbleToDouble()
{
	U64 empty = getEmpty();
	const U64 rank5 = U64(0x000000ff00000000);
	U64 emptyRank6 = nortOne(empty & rank5) & empty;
	return nortOne(emptyRank6) & pieces[BLACK_BISHOP];
}

U64 Board::whitePawnTargets(int squareIndex)
{
	U64 result = 0ULL;
	U64 mask = 1ULL << squareIndex;
	const U64 rank2 = U64(0x000000000000FF00);

	pieces[EMPTY] = getEmpty();

	result |= nortOne(mask) & pieces[EMPTY];
	result |= nortOne(nortOne(mask & rank2) & pieces[EMPTY]) & pieces[EMPTY];
	result |= noWeOne(mask) & getBlack();
	result |= noEaOne(mask) & getBlack();

	if ((noWeOne(mask) & (1ULL << enpassanteIndex)) || (noEaOne(mask) & (1ULL << enpassanteIndex)))
	{
		result |= 1ULL << enpassanteIndex;
	}

	return result;
}

U64 Board::blackPawnTargets(int squareIndex)
{
	U64 result = 0ULL;
	U64 mask = 1ULL << squareIndex;
	const U64 rank7 = U64(0x00ff000000000000);
	pieces[EMPTY] = getEmpty();

	result |= soutOne(mask) & pieces[EMPTY];
	result |= soutOne(soutOne(mask & rank7) & pieces[EMPTY]) & pieces[EMPTY];
	result |= soWeOne(mask) & getWhite();
	result |= soEaOne(mask) & getWhite();
	
	if ((soWeOne(mask) & (1ULL << enpassanteIndex)) || (soEaOne(mask) & (1ULL << enpassanteIndex)))
	{
		result |= 1ULL << enpassanteIndex;
	}

	return result;
}

/**********************************
* PAWN ATTACK MASK METHODS
**********************************/
U64 Board::noEaOne(U64 board)
{
	return (board & notHfile) << 9;
}

U64 Board::noWeOne(U64 board)
{
	return (board & notAfile) << 7;
}

U64 Board::soEaOne(U64 board)
{
	return (board & notHfile) >> 7;
}

U64 Board::soWeOne(U64 board)
{
	return (board & notAfile) >> 9;
}

/*
* Method: WHITE_PAWN_EAST_ATTACK
* Author: Bradley Johnston
*
* Description:
*	Return possible East attacks for white pawns
*/
U64 Board::whitePawnEastAttack()
{
	return noEaOne(pieces[WHITE_PAWN]);
}

/*
* Method: WHITE_PAWN_WEST_ATTACK
* Author: Bradley Johnston
*
* Description:
*	Return viable for White pawn West attacks
*/
U64 Board::whitePawnWestAttack()
{
	return noWeOne(pieces[WHITE_PAWN]);
}

/*
* Method: BLACK_PAWN_EAST_ATTACK
* Author: Bradley Johnston
*
* Description:
*	Return possible Black Pawn East attacks
*/
U64 Board::blackPawnEastAttack()
{
	return soEaOne(pieces[BLACK_PAWN]);
}

/*
* Method: BLACK_PAWN_WEST_ATTACK
* Author: Bradley Johnston
*
* Description:
*	Return possible Black pawn west attacks
*/
U64 Board::blackPawnWestAttack()
{
	return soWeOne(pieces[BLACK_PAWN]);
}

/**********************************
* KNIGHT MOVEMENT MASK METHODS
**********************************/
/*
* Method: NO_NO_EA
* Author: Bradley Johnston
*
* Description:
*	Returns possible knight moves up 2 east 1.
*/
U64 Board::noNoEa(U64 board)
{
	return (board << 17) & notAfile;
}

/*
* Method: NO_EA_EA
* Author: Bradley Johnston
*
* Description:
*	Returns possible knight moves up 1 east 2
*/
U64 Board::noEaEa(U64 board)
{
	return (board << 10) & notABfile;
}

/*
* Method: SO_EA_EA
* Author: Bradley Johnston
*
* Description:
*	Returns possible knight moves down 1 east 2
*/
U64 Board::soEaEa(U64 board)
{
	return (board >> 6) & notABfile;
}

/*
* Method: SO_SO_EA
* Author: Bradley Johnston
*
* Description:
*	Returns possible knight moves down 2 east 1
*/
U64 Board::soSoEa(U64 board)
{
	return (board >> 15) & notAfile;
}

/*
* Method: NO_NO_WE
* Author: Bradley Johnston
*
* Description:
*	returns possible knight moves up 2 west 1
*/
U64 Board::noNoWe(U64 board)
{
	return (board << 15) & notHfile;
}

/*
* Method: NO_WE_WE
* Author: Bradley Johnston
*
* Description:
*	Returns possible knight moves up 1 west 2
*/
U64 Board::noWeWe(U64 board)
{
	return (board << 6) & notGHfile;
}

/*
* Method: SO_WE_WE
* Author: Bradley Johnston
*
* Description:
*	returns possible knight moves down 1 west 2
*/
U64 Board::soWeWe(U64 board)
{
	return (board >> 10) & notGHfile;
}

/*
* Method: SO_SO_WE
* Author: Bradley Johnston
*
* Description:
*	returns possible knight moves down 2 west 1
*/
U64 Board::soSoWe(U64 board)
{
	return (board >> 17) & notHfile;
}

U64 Board::getKnightAttacks(int squareIndex)
{
	U64 result = 0ULL;
	U64 mask = 1ULL << squareIndex;
	bool isWhite = getSquareType(squareIndex) == WHITE_KNIGHT;
	U64 enemyMask = isWhite ? getBlack() : getWhite();
	U64 targetableSquares = enemyMask | getEmpty();

	result |= noNoEa(mask);
	result |= noNoWe(mask);
	result |= noWeWe(mask);
	result |= noEaEa(mask);
	result |= soWeWe(mask);
	result |= soEaEa(mask);
	result |= soSoWe(mask);
	result |= soSoEa(mask);
	result &= targetableSquares;

	return result;
}

/**********************************
* SLIDING PIECE MASK METHODS
**********************************/
/*
*
*/
U64 Board::diagnolAttacks(U64 occ, int sqIndex)
{
	bool isWhite = getSquareType(sqIndex) < BLACK_PAWN;
	U64 targetableSquares = getEmpty() | (isWhite ? getBlack() : getWhite());
	U64* aptr = mBishopTbl[sqIndex].ptr;
	occ &= mBishopTbl[sqIndex].mask;
	occ *= mBishopTbl[sqIndex].magic;
	occ >>= mBishopTbl[sqIndex].shift;
	return aptr[occ] & targetableSquares;
}

/*
* https://www.chessprogramming.org/Magic_Bitboards#Fancy
*/
U64 Board::horizontalAttacks(U64 occ, int sqIndex)
{
	bool isWhite = getSquareType(sqIndex) < BLACK_PAWN;
	U64 targetableSquares = getEmpty() | (isWhite ? getBlack() : getWhite());
	U64* aptr = mRookTbl[sqIndex].ptr;
	occ &= mRookTbl[sqIndex].mask;
	occ *= mRookTbl[sqIndex].magic;
	occ >>= mRookTbl[sqIndex].shift;

	return aptr[occ] & targetableSquares;
}

U64 Board::getKingAttacks(int squareIndex)
{
	bool isWhite = getSquareType(squareIndex) == WHITE_KING;
	U64 mask = 1ULL << squareIndex;
	U64 result = 0ULL;
	U64 targetableSquares = getEmpty() | (isWhite ? getBlack() : getWhite());
	pieces[EMPTY] = getEmpty();
	// Normal movement
	result |= (nortOne(mask)
		| soutOne(mask)
		| eastOne(mask)
		| westOne(mask)
		| noWeOne(mask)
		| noEaOne(mask)
		| soWeOne(mask)
		| soEaOne(mask)) & targetableSquares;

	if (isWhite)
	{
		if (castleRights & WHITE_KING_CASTLE_RIGHTS && pieces[WHITE_ROOK] & (1ULL << 7))
		{
			mask <<= 1;
			//mask |= mask << 1;
			// Test if there is a check in the path.
			if (!isAttacked(4, isWhite) &&
				!isAttacked(5, isWhite) &&
				!isAttacked(6, isWhite) &&
				(mask & pieces[EMPTY]) &&
				((mask << 1) & pieces[EMPTY]))
			{

				result |= 1ULL << 6;
			}

		}
		if (castleRights & WHITE_QUEEN_CASTLE_RIGHTS && pieces[WHITE_ROOK] & (1ULL))
		{
			mask = 1ULL << (-1) + squareIndex;
			//mask |= (mask >> 1) | (mask >> 2);
			if (!isAttacked(4, isWhite) &&
				!isAttacked(3, isWhite) &&
				!isAttacked(2, isWhite) &&
				(mask & pieces[EMPTY]) &&
				((mask >> 1) & pieces[EMPTY]) &&
				((mask >> 2) & pieces[EMPTY]))
			{
				result |= 1ULL << 2;
			}

		}
	}
	else
	{
		if (castleRights & BLACK_KING_CASTLE_RIGHTS && pieces[BLACK_ROOK] & (1ULL << 63))
		{
			mask = 1ULL << (1 + squareIndex);
			//mask |= mask << 1;
			if (!isAttacked(60, isWhite) &&
				!isAttacked(61, isWhite) &&
				!isAttacked(62, isWhite) &&
				(mask & pieces[EMPTY]) &&
				((mask << 1) & pieces[EMPTY]))
			{
				result |= (1ULL << 62);
			}

		}
		if (castleRights & BLACK_QUEEN_CASTLE_RIGHTS && pieces[BLACK_ROOK] & (1ULL << 56))
		{
			mask = 1ULL << (-1 + squareIndex);
			//mask |= mask >> 1 | mask >> 2;
			if (!isAttacked(60, isWhite) &&
				!isAttacked(59, isWhite) &&
				!isAttacked(58, isWhite) &&
				(mask & pieces[EMPTY]) &&
				((mask >> 1) & pieces[EMPTY]) &&
				((mask >> 2) & pieces[EMPTY]))
			{
				result |= (1ULL << 58);
			}
		}
	}

	return result;
}

//U64 Board::diagnolMoves(int sqIndex)
//{
//	bool isWhitePieces = getSquareType(sqIndex) < 7;
//	U64 result = 0ULL;
//	U64 validSquares = isWhitePieces ? ~getWhite() : ~getBlack();
//	U64 enemySquares = isWhitePieces ? getBlack() : getWhite();
//	validSquares |= enemySquares;
//	U64 mask = 1ULL << sqIndex;
//	int maskIndex = sqIndex;
//	int maskRow = maskIndex / 8;
//	int maskCol = maskIndex % 8;
//	bool hasNotHitEnemyFlag = true;
//
//	// NW
//	mask <<= 7;
//	maskIndex = sqIndex + 7;
//	hasNotHitEnemyFlag = true;
//	maskRow = sqIndex / 8;
//	maskCol = sqIndex % 8;
//
//	while (hasNotHitEnemyFlag &&
//		(mask & validSquares) != 0 &&
//		maskIndex / 8 == maskRow + 1 &&
//		maskIndex % 8 == maskCol - 1)
//	{
//		result |= mask;
//
//		if (mask & enemySquares)
//		{
//			hasNotHitEnemyFlag = false;
//		}
//
//		mask <<= 7;
//		maskIndex += 7;
//		maskRow += 1;
//		maskCol -= 1;
//	}
//
//	// NE
//	mask = 1ULL << (sqIndex + 9);
//	maskIndex = sqIndex + 9;
//	maskRow = sqIndex / 8;
//	maskCol = sqIndex % 8;
//	hasNotHitEnemyFlag = true;
//
//	while (hasNotHitEnemyFlag &&
//		(mask & validSquares) != 0 &&
//		maskIndex / 8 == maskRow + 1 &&
//		maskIndex % 8 == maskCol + 1)
//	{
//		result |= mask;
//
//		if (mask & enemySquares)
//		{
//			hasNotHitEnemyFlag = false;
//		}
//
//		mask <<= 9;
//		maskIndex += 9;
//		maskRow += 1;
//		maskCol += 1;
//	}
//
//	// SE
//	mask = 1ULL << (sqIndex - 7);
//	maskIndex = sqIndex - 7;
//	maskRow = sqIndex / 8;
//	maskCol = sqIndex % 8;
//	hasNotHitEnemyFlag = true;
//
//	while (hasNotHitEnemyFlag &&
//		(mask & validSquares) != 0 &&
//		maskIndex / 8 == maskRow - 1 &&
//		maskIndex % 8 == maskCol + 1)
//	{
//		result |= mask;
//
//		if (mask & enemySquares)
//		{
//			hasNotHitEnemyFlag = false;
//		}
//
//		mask >>= 7;
//		maskIndex -= 7;
//		maskRow -= 1;
//		maskCol += 1;
//	}
//
//	// SW
//	mask = 1ULL << (sqIndex - 9);
//	maskIndex = sqIndex - 9;
//	maskRow = sqIndex / 8;
//	maskCol = sqIndex % 8;
//	hasNotHitEnemyFlag = true;
//
//	while (hasNotHitEnemyFlag &&
//		(mask & validSquares) != 0 &&
//		maskIndex / 8 == maskRow - 1 &&
//		maskIndex % 8 == maskCol - 1)
//	{
//		result |= mask;
//
//		if (mask & enemySquares)
//		{
//			hasNotHitEnemyFlag = false;
//		}
//
//		mask >>= 9;
//		maskIndex -= 9;
//		maskRow -= 1;
//		maskCol -= 1;
//	}
//
//	return result;
//}
//
//U64 Board::horizontalMoves(int sqIndex)
//{
//	bool isWhitePieces = getSquareType(sqIndex) < 7;
//	U64 result = 0ULL;
//	U64 validSquares = isWhitePieces ? ~getWhite() : ~getBlack();
//	U64 enemySquares = isWhitePieces ? getBlack() : getWhite();
//	validSquares |= enemySquares;
//	U64 mask = 1ULL << sqIndex;
//	int maskIndex = sqIndex;
//	bool hasNotHitEnemyFlag = true;
//
//	// Generate up
//	maskIndex += 8;
//	mask <<= 8;
//	hasNotHitEnemyFlag = true;
//
//	while (maskIndex < 63 &&
//		hasNotHitEnemyFlag &&
//		(mask & validSquares) != 0)
//	{
//		result |= mask;
//
//		// Test hitEnemyFlag
//		if (mask & enemySquares)
//		{
//			hasNotHitEnemyFlag = false;
//		}
//
//		mask <<= 8;
//		maskIndex += 8;
//	}
//
//	// Generate east
//	maskIndex = sqIndex - 1;
//	mask = 1ULL << (sqIndex - 1);
//	hasNotHitEnemyFlag = true;
//
//	while (maskIndex / 8 == sqIndex / 8 &&
//		hasNotHitEnemyFlag &&
//		validSquares &&
//		(mask & validSquares) != 0)
//	{
//		result |= mask;
//
//		// Test hitEnemyFlag
//		if (mask & enemySquares)
//		{
//			hasNotHitEnemyFlag = false;
//		}
//
//		mask >>= 1;
//		maskIndex -= 1;
//	}
//
//	// Generate down
//	maskIndex = sqIndex - 8;
//	mask = 1ULL << maskIndex;
//	hasNotHitEnemyFlag;
//
//	while (maskIndex > 0 &&
//		hasNotHitEnemyFlag &&
//		(mask & validSquares) != 0)
//	{
//		result |= mask;
//
//		// test hitEnemyFlag
//		if (mask & enemySquares)
//		{
//			hasNotHitEnemyFlag = false;
//		}
//
//		maskIndex -= 8;
//		if (maskIndex > 0)
//			mask >>= 8;
//	}
//
//	// Generate west
//	maskIndex = sqIndex + 1;
//	mask = 1ULL << maskIndex;
//	hasNotHitEnemyFlag = true;
//
//	while (maskIndex / 8 == sqIndex / 8 &&
//		hasNotHitEnemyFlag &&
//		validSquares &&
//		(mask & validSquares) != 0)
//	{
//		result |= mask;
//
//		// test hitEnemyFlag
//		if (mask & enemySquares)
//		{
//			hasNotHitEnemyFlag = false;
//		}
//
//		maskIndex += 1;
//		mask <<= 1;
//	}
//
//	return result;
//}

/**********************************
* ATTACK MASKS METHODS
**********************************/
/*
* Method: ATTACKS_TO
* Author: Bradley Johnston
*
* Description:
*	Generate a list of all squares attacking target square
*/
U64 Board::attacksFrom(int squareIndex)
{
	U64 result = 0ULL;
	int pieceType = getSquareType(squareIndex);

	switch (pieceType)
	{
	case EMPTY:
		break;
	case WHITE_PAWN:
		return whitePawnTargets(squareIndex);
		break;
	case BLACK_PAWN:
		return blackPawnTargets(squareIndex);
		break;
	case WHITE_KNIGHT:
	case BLACK_KNIGHT:
		return getKnightAttacks(squareIndex);
		break;
	case WHITE_BISHOP:
	case BLACK_BISHOP:
		return diagnolAttacks(~getEmpty(), squareIndex);
		break;
	case WHITE_ROOK:
	case BLACK_ROOK:
		return horizontalAttacks(~getEmpty(), squareIndex);
		break;
	case WHITE_QUEEN:
	case BLACK_QUEEN:
		return horizontalAttacks(~getEmpty(), squareIndex) | diagnolAttacks(~getEmpty(), squareIndex);
		break;
	case WHITE_KING:
	case BLACK_KING:
		return getKingAttacks(squareIndex);
	default:
		return 0ULL;
		break;
	}
	return result;
}

/*
* Method: IS_ATTACKED
* Author: Bradley Johnston
*
* Description:
*	Calculates if a square is being attacked.
*/
bool Board::isAttacked(int squareIndex, bool isPieceWhite)
{
	U64 mask = 1ULL << squareIndex;
	U64 occupied = ~getEmpty();

	// Generate Pawn attacks
	if (isPieceWhite)
	{
		if (blackPawnEastAttack() & mask ||
			blackPawnWestAttack() & mask)
		{
			return true;
		}
	}
	if (!isPieceWhite)
	{
		if (whitePawnEastAttack() & mask ||
			whitePawnWestAttack() & mask)
		{
			return true;
		}
	}
	
	// Generate Knight attacks
	if (noNoEa(pieces[!isPieceWhite ? WHITE_KNIGHT : BLACK_KNIGHT]) & mask)
	{
		return true;
	}
	if (noEaEa(pieces[!isPieceWhite ? WHITE_KNIGHT : BLACK_KNIGHT]) & mask)
	{
		return true;
	}
	if (soEaEa(pieces[!isPieceWhite ? WHITE_KNIGHT : BLACK_KNIGHT]) & mask)
	{
		return true;
	}
	if (soSoEa(pieces[!isPieceWhite ? WHITE_KNIGHT : BLACK_KNIGHT]) & mask)
	{
		return true;
	}
	if (noNoWe(pieces[!isPieceWhite ? WHITE_KNIGHT : BLACK_KNIGHT]) & mask)
	{
		return true;
	}
	if (noWeWe(pieces[!isPieceWhite ? WHITE_KNIGHT : BLACK_KNIGHT]) & mask)
	{
		return true;
	}
	if (soWeWe(pieces[!isPieceWhite ? WHITE_KNIGHT : BLACK_KNIGHT]) & mask)
	{
		return true;
	}
	if (soSoWe(pieces[!isPieceWhite ? WHITE_KNIGHT : BLACK_KNIGHT]) & mask)
	{
		return true;
	}

	// Generate Bishop Attacks
	U64 buffer = pieces[!isPieceWhite ? WHITE_BISHOP : BLACK_BISHOP];
	int index = 0;

	while (buffer > 0ULL)
	{
		if ((buffer & 1ULL) == 1ULL)
		{
			if (diagnolAttacks(occupied, index) & mask)
			{
				return true;
			}
		}
		buffer >>= 1;
		index += 1;
	}

	// Generate Rook Attacks
	buffer = pieces[!isPieceWhite ? WHITE_ROOK : BLACK_ROOK];
	index = 0;

	while (buffer > 0ULL)
	{
		if ((buffer & 1ULL) == 1ULL)
		{
			if (horizontalAttacks(occupied, index) & mask)
			{
				return true;
			}
		}
		buffer >>= 1;
		index += 1;
	}

	// Generate Queen Attacks
	buffer = pieces[!isPieceWhite ? WHITE_QUEEN : BLACK_QUEEN];
	index = 0;

	while (buffer > 0ULL)
	{
		if ((buffer & 1ULL) == 1ULL)
		{
			if (diagnolAttacks(occupied, index) & mask ||
				horizontalAttacks(occupied, index) & mask)
			{
				return true;
			}
		}
		buffer >>= 1;
		index += 1;
	}

	// Generate King Attacks
	buffer = pieces[!isPieceWhite ? WHITE_KING : BLACK_KING];
	index = 0;
	int targetRow = squareIndex / 8;
	int targetCol = squareIndex % 8;

	while (buffer > 0ULL)
	{
		if ((buffer & 1ULL) == 1ULL)
		{
			int row = index / 8;
			int col = index % 8;
			int rowDiff = row - targetRow;
			int colDiff = col - targetCol;
			if (rowDiff <= 1 && rowDiff >= -1 && colDiff <= 1 && colDiff >= -1)
			{
				return true;
			}
		}
		buffer >>= 1;
		index += 1;
	}

	return false;
}

int Board::U64toIndex(U64 board)
{
	int row, col;

	for (row = 0; row <= 7; row++)
	{
		for (col = 0; col <= 7; col++)
		{
			if (board & (1ULL << (col + row * 8)))
			{
				return col + row * 8;
			}
		}
	}

	return 0ULL;
}


/*
struct Move
{
	int startingSquare;
	int targetSquare;
	int previousEnpassantIndex;
	int previousHalfCount;
	int previousFullCount;
	CastleRights previousCastleRights;
};
*/
list<Move> Board::generateMoves()
{
	list<Move> result = list<Move>();

	pieces[EMPTY] = getEmpty();
	Move buffer;
	Move copy;
	U64 attacksBuffer;
	buffer.previousCastleRights = castleRights;
	buffer.previousEnpassantIndex = enpassanteIndex;
	buffer.previousHalfCount = halfMoveCount;
	buffer.previousFullCount = fullMoveCount;
	buffer.isWhite = isWhiteTurn;
	buffer.promotionPieceType = EMPTY;
	buffer.previousBoards = boardsSinceCapture;


	for (int i = 0; i < 64; i++)
	{
		if ((getSquareType(i) <= WHITE_KING) == isWhiteTurn)
		{
			buffer.previousCastleRights = castleRights;
			attacksBuffer = attacksFrom(i);
			buffer.startingSquare = i;
			for (int j = 0; attacksBuffer != 0ULL; attacksBuffer >>= 1, j++)
			{
				buffer.capturedEnpassant = false;
				if (1ULL & attacksBuffer)
				{
					int pieceType = getSquareType(i);
					if (pieceType == WHITE_PAWN)
					{
						if (j - i == 7 ||
							j - i == 9)
						{
							if (getSquareType(j) == EMPTY)
							{
								buffer.capturedEnpassant = true;
							}
						}
					}
					if (pieceType == BLACK_PAWN)
					{


						if (i - j == 9 ||
							i - j == 7)
						{
							if (getSquareType(j) == EMPTY)
							{
								buffer.capturedEnpassant = true;
							}
						}
					}

					buffer.targetSquare = j;
					buffer.previousTargetSquarePiece = getSquareType(j);
					if (makeMove(buffer))
					{
						unmakeMove(buffer);

						copy = buffer;
						if (pieceType == WHITE_PAWN && (buffer.targetSquare / 8) == 7)
						{
							buffer.promotionPieceType = WHITE_KNIGHT;
							copy = buffer;
							result.push_back(copy);

							buffer.promotionPieceType = WHITE_BISHOP;
							copy = buffer;
							result.push_back(copy);

							buffer.promotionPieceType = WHITE_ROOK;
							copy = buffer;
							result.push_back(copy);

							buffer.promotionPieceType = WHITE_QUEEN;
							copy = buffer;
							result.push_back(copy);
						}
						else if (pieceType == BLACK_PAWN && (buffer.targetSquare / 8) == 0)
						{
							buffer.promotionPieceType = BLACK_KNIGHT;
							copy = buffer;
							result.push_back(copy);

							buffer.promotionPieceType = BLACK_BISHOP;
							copy = buffer;
							result.push_back(copy);

							buffer.promotionPieceType = BLACK_ROOK;
							copy = buffer;
							result.push_back(copy);

							buffer.promotionPieceType = BLACK_QUEEN;
							copy = buffer;
							result.push_back(copy);
						}
						else
						{
							result.push_back(copy);
						}
						buffer.promotionPieceType = EMPTY;
					}
				}
			}
		}
	}

	return result;
}

/**************************************************
* ################# PUBLIC METHODS ################
**************************************************/
/**********************************
* CONSTRUCTORS
**********************************/
/*
* Method: BOARD
* Author: Bradley Johnston
* 
* Description:
*	Default Constructor. Loads board into the 
* default start game state.
*/
Board::Board()
{
	for (int i = 0; i < 15; i++)
	{
		pieces[i] = 0;
	}

	isWhiteTurn = true;
	castleRights = 0x0f;
	halfMoveCount = 0;
	fullMoveCount = 0;
	enpassanteIndex = 99;
	//generateMagicTables();
	mBishopTbl = new SMagic[64];
	mRookTbl = new SMagic[64];
	loadMagicTables();
	loadBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -- 0 0");
	isOriginal = true;
	capturedPieceLastTurn = false;
	boardsSinceCapture = map<string, int>();
}

/*
* Method: COPY_CONSTRUCTOR
* Author: Bradley Johnston
*
* Description:
*	Copy Constructor.
*/
Board::Board(Board& rhs)
{
	for (int i = 0; i < 15; i++)
	{

		pieces[i] = rhs.pieces[i];
	}

	isWhiteTurn = rhs.isWhiteTurn;
	castleRights = rhs.castleRights;
	halfMoveCount = rhs.halfMoveCount;
	fullMoveCount = rhs.fullMoveCount;
	enpassanteIndex = rhs.enpassanteIndex;
	mBishopTbl = &(rhs.mBishopTbl[0]);
	mRookTbl = &(rhs.mRookTbl[0]);
	attackTable = &(rhs.attackTable[0]);
	isOriginal = false;
	capturedPieceLastTurn = rhs.capturedPieceLastTurn;

	for (std::map<string, int>::iterator it = rhs.boardsSinceCapture.begin(); it != rhs.boardsSinceCapture.end(); it++)
	{
		boardsSinceCapture[it->first] = it->second;
	}
}

/*
* Method: DESTRUCTOR
* Author: Bradley Johnston
*
* Description:
*	Default Destructor
*/
Board::~Board()
{
	if (isOriginal)
	{
		delete[] mBishopTbl;
		delete[] mRookTbl;
		delete[] attackTable;
	}

}

/**********************************
* DISPLAY METHODS
**********************************/
/*
* Method: DISPLAY_CONSOLE
* Author: Bradley Johnston
*
* Description:
*	Converts the board state to a string and displays it to the user.
*/
void Board::displayConsole()
{
	string board = "";
	U64 mask = 0ULL;
	for (int row = 7; row >= 0; row--)
	{
		board += (char)(57 - (8 - row));
		board += "| ";
		for (int col = 0; col < 8; col++)
		{
			mask = 1ULL << ((8 * row) + col);
			if ((pieces[WHITE_PAWN] & mask) > 0ULL)
			{
				board += "P";
			}
			else if ((pieces[WHITE_KNIGHT] & mask) > 0ULL)
			{
				board += "N";
			}
			else if ((pieces[WHITE_BISHOP] & mask) > 0ULL)
			{
				board += "B";
			}
			else if ((pieces[WHITE_ROOK] & mask) > 0ULL)
			{
				board += "R";
			}
			else if ((pieces[WHITE_QUEEN] & mask) > 0ULL)
			{
				board += "Q";
			}
			else if ((pieces[WHITE_KING] & mask) > 0ULL)
			{
				board += "K";
			}
			else if ((pieces[BLACK_PAWN] & mask) > 0ULL)
			{
				board += "p";
			}
			else if ((pieces[BLACK_KNIGHT] & mask) > 0ULL)
			{
				board += "n";
			}
			else if ((pieces[BLACK_BISHOP] & mask) > 0ULL)
			{
				board += "b";
			}
			else if ((pieces[BLACK_ROOK] & mask) > 0ULL)
			{
				board += "r";
			}
			else if ((pieces[BLACK_QUEEN] & mask) > 0ULL)
			{
				board += "q";
			}
			else if ((pieces[BLACK_KING] & mask) > 0ULL)
			{
				board += "k";
			}
			else
			{
				board += ".";
			}
			board += " ";
		}
		board += "\n";
	}
	board += "-+-----------------\n | A B C D E F G H";
	std::cout << board << endl << endl;
}

/**********************************
* LOAD/SAVE METHODS
**********************************/
/*
* Method: LOAD_BOARD
* Author: Bradley Johnston
*
* Description:
*	Loads a board state by the FEN code.
* The FEN code has multiple parts, first is the piece positions
* where the positions are determined by characters and numbers
* 2k2P1p/ = . . k . . P . p/
* 
* Afterwards you have the current turn
* 
* Then castling rights
* 
* Enpassant targets
* 
* Half-move count
* 
* Full-move count
*/
void Board::loadBoard(string FEN)
{
	for (int i = 0; i < 13; i++)
	{
		pieces[i] = 0ULL;
	}

	// For each letter in the FEN string handle it
	unsigned int index = 0;
	int row = 0;
	int offset = 0;
	unsigned char buffer[13];
	unsigned char mask = 0x80; // 1000 0000
	for (int i = 0; i < 13; i++)
	{
		buffer[i] = 0;
	}

	// Until the first space
	while (FEN[index] != ' ')
	{
		// If it is a number
		if (isdigit(FEN[index]))
		{
			int num = 9 - (57 - (int)FEN[index]);

			for (int i = 0; i < num; i++)
			{
				buffer[0] |= mask;
				buffer[0] >>= 1;
			}

			for (int i = 1; i < 13; i++)
			{
				buffer[i] >>= (num);
			}
		}
		// If it is a letter
		else if (isalpha(FEN[index]))
		{
			for (int i = 0; i < 13; i++)
			{
				buffer[i] >>= 1;
			}
			switch (FEN[index])
			{
			case 'P':
				buffer[WHITE_PAWN] |= mask;
				break;
			case 'N':
				buffer[WHITE_KNIGHT] |= mask;
				break;
			case 'B':
				buffer[WHITE_BISHOP] |= mask;
				break;
			case 'R':
				buffer[WHITE_ROOK] |= mask;
				break;
			case 'Q':
				buffer[WHITE_QUEEN] |= mask;
				break;
			case 'K':
				buffer[WHITE_KING] |= mask;
				break;
			case 'p':
				buffer[BLACK_PAWN] |= mask;
				break;
			case 'n':
				buffer[BLACK_KNIGHT] |= mask;
				break;
			case 'b':
				buffer[BLACK_BISHOP] |= mask;
				break;
			case 'r':
				buffer[BLACK_ROOK] |= mask;
				break;
			case 'q':
				buffer[BLACK_QUEEN] |= mask;
				break;
			case 'k':
				buffer[BLACK_KING] |= mask;
				break;
			default:
				cerr << "Unexpected letter" << endl;
			}

			
		}
		// If it is a '/'
		else if (FEN[index] == '/')
		{
			for (int i = 0; i < 13; i++)
			{
				pieces[i] <<= 8;
				pieces[i] |= buffer[i];
				buffer[i] = 0;
			}
		}
		else
		{
			cerr << "Received unexpected token: " << FEN[index] << endl;
		}
		index++;
	}
	// Add last row to pieces
	for (int i = 0; i < 13; i++)
	{
		pieces[i] <<= 8;
		pieces[i] |= buffer[i];
	}
	index++;
	isWhiteTurn = FEN[index] == 'w';
	index += 2;

	castleRights = 0;
	// Castling rights
	while (FEN[index] != ' ')
	{
		if (FEN[index] == 'k')
		{
			castleRights |= BLACK_KING_CASTLE_RIGHTS;
		}
		else if (FEN[index] == 'q')
		{
			castleRights |= BLACK_QUEEN_CASTLE_RIGHTS;
		}
		else if (FEN[index] == 'K')
		{
			castleRights |= WHITE_KING_CASTLE_RIGHTS;
		}
		else if (FEN[index] == 'Q')
		{
			castleRights |= WHITE_QUEEN_CASTLE_RIGHTS;
		}

		index++;
	}

	index++;
	enpassanteIndex = 0;
	if (FEN[index] != '-')
	{
		// Enpassante
		if (tolower(FEN[index]) == 'a');
		else if (tolower(FEN[index]) == 'b')
		{
			enpassanteIndex += 1;
		}
		else if (tolower(FEN[index]) == 'c')
		{

			enpassanteIndex += 2;
		}
		else if (tolower(FEN[index]) == 'd')
		{

			enpassanteIndex += 3;
		}
		else if (tolower(FEN[index]) == 'e')
		{

			enpassanteIndex += 4;
		}
		else if (tolower(FEN[index]) == 'f')
		{

			enpassanteIndex += 5;
		}
		else if (tolower(FEN[index]) == 'g')
		{

			enpassanteIndex += 6;
		}
		else if (tolower(FEN[index]) == 'h')
		{

			enpassanteIndex += 7;
		}
		index++;
		if (FEN[index] == '3')
		{
			enpassanteIndex += 16;
		}
		if (FEN[index] == '6')
		{
			enpassanteIndex += 40;
		}
	}
	else
	{
		enpassanteIndex = 99;
	}
	index += 2;

	// Halfmove clock
	string moveBuffer = "";
	while (FEN[index] != ' ')
	{
		moveBuffer += FEN[index];
		index++;
	}
	halfMoveCount = atoi(moveBuffer.c_str());

	index++;

	// Full move counter
	moveBuffer = "";
	while (index < FEN.length())
	{
		moveBuffer += FEN[index];
		index++;
	}
	fullMoveCount = atoi(moveBuffer.c_str());

	boardsSinceCapture.clear();
	boardsSinceCapture[saveBoardMinusFEN()] = 1;
}

string Board::saveBoard()
{
	string result = "";
	int emptyCount = 0;
	int piece;

	// Board section
	pieces[EMPTY] = getEmpty();
	for (int r = 7; r >= 0; r--)
	{
		for (int c = 0; c < 8; c++)
		{
			piece = getSquareType(c + r * 8);

			if (piece == EMPTY)
			{
				emptyCount += 1;
			}
			else if (emptyCount > 0)
			{
				result += (char)(emptyCount + 48);
				emptyCount = 0;
			}

			switch (piece)
			{
			case WHITE_PAWN:
				result += 'P';
				break;
			case WHITE_KNIGHT:
				result += 'N';
				break;
			case WHITE_BISHOP:
				result += 'B';
				break;
			case WHITE_ROOK:
				result += 'R';
				break;
			case WHITE_QUEEN:
				result += 'Q';
				break;
			case WHITE_KING:
				result += 'K';
				break;
			case BLACK_PAWN:
				result += 'p';
				break;
			case BLACK_KNIGHT:
				result += 'n';
				break;
			case BLACK_BISHOP:
				result += 'b';
				break;
			case BLACK_ROOK:
				result += 'r';
				break;
			case BLACK_QUEEN:
				result += 'q';
				break;
			case BLACK_KING:
				result += 'k';
				break;
			case EMPTY:
				break;
			default:
				cerr << "Unknown piece type " << piece << "\n";
				break;
			}
		}

		if (emptyCount > 0)
		{
			result += (char)(emptyCount + 48);
			emptyCount = 0;
		}

		if (r != 0)
			result += '/';
	}

	// Turn
	result += ' ';
	result += (isWhiteTurn ? 'w' : 'b');
	result += ' ';

	// Castling Rights
	if (castleRights & WHITE_KING_CASTLE_RIGHTS)
	{
		result += 'K';
	}
	if (castleRights & WHITE_QUEEN_CASTLE_RIGHTS)
	{
		result += 'Q';
	}
	if (castleRights & BLACK_KING_CASTLE_RIGHTS)
	{
		result += 'k';
	}
	if (castleRights & BLACK_QUEEN_CASTLE_RIGHTS)
	{
		result += 'q';
	}
	if (castleRights == (unsigned char)0)
	{
		result += '-';
	}

	result += ' ';

	// Enpassant Index
	int enpassanteRow = enpassanteIndex / 8;
	int enpassanteCol = enpassanteIndex % 8;

	if (enpassanteIndex > 64)
	{
		result += '-';
	}
	else
	{
		result += (char)(enpassanteCol + 98);
		result += (char)(enpassanteRow + 48);
	}

	result += ' ';

	// halfmove tracking
	result += std::to_string(halfMoveCount);
	result += ' ';

	// Fullmove tracking
	result += std::to_string(fullMoveCount);

	return result;
}

string Board::saveBoardMinusFEN()
{
	string result = "";
	int emptyCount = 0;
	int piece;

	// Board section
	pieces[EMPTY] = getEmpty();
	for (int r = 7; r >= 0; r--)
	{
		for (int c = 0; c < 8; c++)
		{
			piece = getSquareType(c + r * 8);

			if (piece == EMPTY)
			{
				emptyCount += 1;
			}
			else if (emptyCount > 0)
			{
				result += (char)(emptyCount + 48);
				emptyCount = 0;
			}

			switch (piece)
			{
			case WHITE_PAWN:
				result += 'P';
				break;
			case WHITE_KNIGHT:
				result += 'N';
				break;
			case WHITE_BISHOP:
				result += 'B';
				break;
			case WHITE_ROOK:
				result += 'R';
				break;
			case WHITE_QUEEN:
				result += 'Q';
				break;
			case WHITE_KING:
				result += 'K';
				break;
			case BLACK_PAWN:
				result += 'p';
				break;
			case BLACK_KNIGHT:
				result += 'n';
				break;
			case BLACK_BISHOP:
				result += 'b';
				break;
			case BLACK_ROOK:
				result += 'r';
				break;
			case BLACK_QUEEN:
				result += 'q';
				break;
			case BLACK_KING:
				result += 'k';
				break;
			case EMPTY:
				break;
			default:
				cerr << "Unknown piece type " << piece << "\n";
				break;
			}
		}

		if (emptyCount > 0)
		{
			result += (char)(emptyCount + 48);
			emptyCount = 0;
		}

		if (r != 0)
			result += '/';
	}

	return result;
}

/**********************************
* BOARD INTERACTION METHODS
**********************************/
/*
* Method: MOVE_PIECE
* Author: Bradley Johnston
*
* Description:
*	Attempt to move a piece from the first variable location to the second variable location.
*/
bool Board::movePiece(int squareIndex, int targetSquareIndex, bool force)
{
	int pieceType = getSquareType(squareIndex);
	U64 mask = 1ULL << squareIndex;
	U64 targetMask = 1ULL << targetSquareIndex;
	bool isValidMove = false;
	bool isEnpassant = false;
	CastleRights previousCastle = castleRights;

	if ((isWhiteTurn && pieceType >= 7) ||
		(!isWhiteTurn && pieceType <= 6))
	{
		//cerr << "Not your turn" << endl;
		return false;
	}

	switch (pieceType)
	{
	case WHITE_PAWN:
		if (moveWhitePawn(squareIndex, targetSquareIndex))
		{
			isValidMove = true;
			if (targetSquareIndex == enpassanteIndex)
			{
				isEnpassant = true;
			}
		}
		break;
	case BLACK_PAWN:
		if (moveBlackPawn(squareIndex, targetSquareIndex))
		{
			isValidMove = true;
			if (targetSquareIndex == enpassanteIndex)
			{
				isEnpassant = true;
			}
		}
		break;
	case WHITE_KNIGHT:
	case BLACK_KNIGHT:
		if (moveKnight(squareIndex, targetSquareIndex))
		{
			isValidMove = true;
		}
		break;
	case WHITE_BISHOP:
	case BLACK_BISHOP:
		if (moveBishop(squareIndex, targetSquareIndex))
		{
			isValidMove = true;
		}
		break;
	case WHITE_ROOK:
	case BLACK_ROOK:
		if (moveRook(squareIndex, targetSquareIndex))
		{

			if (isWhiteTurn)
			{
				if (squareIndex == 0)
				{
					castleRights &= ~(WHITE_QUEEN_CASTLE_RIGHTS);
				}
				else
				{
					castleRights &= ~(WHITE_KING_CASTLE_RIGHTS);
				}
			}
			else
			{
				if (squareIndex == 63)
				{
					castleRights &= ~(BLACK_KING_CASTLE_RIGHTS);
				}
				else
				{
					castleRights &= ~(BLACK_QUEEN_CASTLE_RIGHTS);
				}
			}

			isValidMove = true;
		}
		break;
	case WHITE_QUEEN:
	case BLACK_QUEEN:
		if (moveQueen(squareIndex, targetSquareIndex))
		{
			isValidMove = true;
		}
		break;
	case WHITE_KING:
	case BLACK_KING:
		if (moveKing(squareIndex, targetSquareIndex))
		{
			isValidMove = true;
			if (isWhiteTurn)
			{
				castleRights &= ~(WHITE_KING_CASTLE_RIGHTS);
				castleRights &= ~(WHITE_QUEEN_CASTLE_RIGHTS);
			}
			else
			{
				castleRights &= ~(BLACK_KING_CASTLE_RIGHTS);
				castleRights &= ~(BLACK_QUEEN_CASTLE_RIGHTS);
			}
			// queen side castling
			if (squareIndex - targetSquareIndex == 2)
			{
				movePiece(squareIndex - 4, squareIndex - 1);
				isWhiteTurn = !isWhiteTurn;
			}
			// king side castling
			if (squareIndex - targetSquareIndex == -2)
			{
				movePiece(squareIndex + 3, squareIndex + 1);
				isWhiteTurn = !isWhiteTurn;
			}
		}
		break;
	case EMPTY:
		return false;
	default:
		break;
	}

	if (isValidMove || force)
	{
		// Test if the move would lead to check
		int targetType = getSquareType(targetSquareIndex);

		if (isEnpassant)
		{
			if (isWhiteTurn)
			{
				targetType = BLACK_PAWN;
				pieces[BLACK_PAWN] &= ~(1ULL << (targetSquareIndex - 8));
			}
			if (!isWhiteTurn)
			{
				targetType = WHITE_PAWN;
				pieces[WHITE_PAWN] &= ~(1ULL << (targetSquareIndex + 8));
			}
		}

		pieces[pieceType] &= ~mask; // remove piece from starting square
		pieces[pieceType] |= targetMask; // place piece on target square
		pieces[targetType] &= ~targetMask; // capture piece on target square

		// Check if the king is checked because of this move
		if (!isChecked())
		{
			isWhiteTurn = !isWhiteTurn;
			if ((pieceType == WHITE_PAWN && targetSquareIndex - squareIndex == 16) ||
				(pieceType == BLACK_PAWN && squareIndex - targetSquareIndex == 16))
			{
				enpassanteIndex = targetSquareIndex + (isWhiteTurn ? 8 : -8);
			}
			else
			{
				enpassanteIndex = 99; // Index out of bounds
			}

			capturedPieceLastTurn = (targetType != EMPTY);

			if (capturedPieceLastTurn || pieceType == WHITE_PAWN || pieceType == BLACK_PAWN)
			{
				fullMoveCount = 0;
				halfMoveCount = 0;
				boardsSinceCapture.clear();
			}
			else
			{
				string boardState = saveBoardMinusFEN();

				if (boardsSinceCapture.count(boardState))
				{
					boardsSinceCapture[boardState] += 1;
				}
				else
				{
					boardsSinceCapture[boardState] = 1;
				}

				halfMoveCount += 1;
				if (halfMoveCount % 2 == 0)
				{
					fullMoveCount += 1;
				}
			}

			return true;
		}
		else
		{
			castleRights = previousCastle;
			pieces[pieceType] |= mask; // replace piece from starting square
			pieces[pieceType] &= ~targetMask; // remove piece from target square
			if (isEnpassant)
			{
				if (isWhiteTurn)
				{
					pieces[BLACK_PAWN] |= 1ULL << (targetSquareIndex - 8);
				}
				else
				{
					pieces[WHITE_PAWN] |= 1ULL << (targetSquareIndex + 8);
				}
			}
			else
			{
				pieces[targetType] |= targetMask; // replace piece on target square
			}
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool Board::makeMove(Move move)
{
	bool result = movePiece(move.startingSquare, move.targetSquare);

	if (result && move.promotionPieceType != EMPTY)
	{
		pieces[getSquareType(move.targetSquare)] &= ~(1ULL << move.targetSquare);
		pieces[move.promotionPieceType] |= (1ULL << move.targetSquare);
	}

	return result;
}

void Board::unmakeMove(Move move)
{
	boardsSinceCapture = move.previousBoards;
	int pieceType = getSquareType(move.targetSquare); // Get moving piece type
	pieces[pieceType] &= ~(1ULL << move.targetSquare); // remove moving piece from target square

	if (move.promotionPieceType != EMPTY)
	{
		if (pieceType > 6)
		{
			pieceType = BLACK_PAWN;
		}
		else
		{
			pieceType = WHITE_PAWN;
		}
	}

	if (move.capturedEnpassant)
	{
		if (pieceType == WHITE_PAWN)
		{
			pieces[BLACK_PAWN] |= (1ULL << (move.targetSquare - 8));
		}
		else
		{
			pieces[WHITE_PAWN] |= (1ULL << (move.targetSquare + 8));
		}
	}
	else
	{
		pieces[move.previousTargetSquarePiece] |= 1ULL << move.targetSquare; // If there is a taken piece replace it
	}



	pieces[pieceType] |= (1ULL << move.startingSquare); // Replace moved piece on source square
	halfMoveCount = move.previousHalfCount; // reset halfmove count
	fullMoveCount = move.previousFullCount; // reset fullmove count
	enpassanteIndex = move.previousEnpassantIndex; // reset enpassant index
	isWhiteTurn = move.isWhite; // alter turn var
	if ((pieceType == WHITE_KING || pieceType == BLACK_KING) &&
		(move.startingSquare - move.targetSquare == 2 || move.startingSquare - move.targetSquare == -2))
	{
		// Queenside uncastling
		if (move.startingSquare - move.targetSquare == 2)
		{
			movePiece(move.startingSquare - 1, move.startingSquare - 4, true);
		}
		if (move.startingSquare - move.targetSquare == -2)
		{
			movePiece(move.startingSquare + 1, move.startingSquare + 3, true);
		}
		isWhiteTurn = move.isWhite;
	}


	castleRights = move.previousCastleRights;
	capturedPieceLastTurn = move.previousTargetSquarePiece != EMPTY;
	return;
}

/*
* Method: GET_MOVE_COUNT
* Author: Bradley Johnston
*
* Description:
*	return the number of possible moves that can be taken
* in addition to the number of moves that can be taken from there.
* up to the depth given
*/
int Board::getMoveCount(Board node, int depth, bool displayPerft)
{
	if (depth == 0)
	{
		return 1;
	}

	if (node.getStateCount() >= 3)
	{
		return 0;
	}

	int count = 0;
	int independantCount = 0;
	list<Move> moves = node.generateMoves();
	Board bufferBoard(node);

	for (std::list<Move>::iterator it = moves.begin(); it != moves.end(); it++)
	{

		independantCount = 0;
		bufferBoard = node;
		if (bufferBoard.makeMove(*it))
		{

			independantCount = bufferBoard.getMoveCount(bufferBoard, depth - 1, false);

			count += independantCount;
		}

		if (displayPerft)
		{
			cout << indexToCoord(it->startingSquare) << indexToCoord(it->targetSquare) << ": " << independantCount << endl;

		}

	}

	return count;
}

float Board::evaluation()
{
	float result = 0;
	float buffer = 0;
	float white_score = 0;
	float black_score = 0;
	int pieceCount = 0;

	float scores[6] = { 1, 3, 3, 5, 8, 99 };

	for (int i = WHITE_PAWN; i <= WHITE_KING; i++)
	{
		pieceCount = count_1s(pieces[i]);
		buffer = (pieceCount * scores[i - 1]);
		white_score += buffer;

		pieceCount = count_1s(pieces[i + WHITE_KING]);
		buffer = -1 * (pieceCount * scores[i - 1]);
		black_score += buffer;
	}
	
	result = white_score + black_score;

	return result;

}

float Board::alphaBetaSearch(int depth, float& alpha, float& beta, bool playingForWhite)
{
	if (depth == 0)
	{
		return evaluation();
	}

	list<Move> moveList = generateMoves();
	if (moveList.size() == 0)
	{
		return (playingForWhite) ? -10000000 : 10000000000;
	}

	float value = 0;
	float bestValue = 0;

	if (playingForWhite)
	{
		value = -99999999; // Negative Infinity
		bestValue = -9999999999; // Negative Infinity

		for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
		{
			makeMove(*it);
			value = alphaBetaSearch(depth - 1, alpha, beta, playingForWhite);
			unmakeMove(*it);
			if (value > bestValue)
			{
				bestValue = value;
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
		value = 999999999; // Positive Infinity
		bestValue = 9999999999;

		for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
		{
			makeMove(*it);
			value = alphaBetaSearch(depth - 1, alpha, beta, playingForWhite);
			unmakeMove(*it);
			if (value < bestValue)
			{
				bestValue = value;
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

void Board::GenerateBestMove(int depth)
{
	list<Move> moveList = generateMoves();

	if (moveList.size() == 0)
	{
		return;
	}

	float value = 0;
	float bestValue = 0;
	Move bestMove;
	float alpha = -99999999;
	float beta = 99999999;

	bestMove.startingSquare = 99;
	bestMove.targetSquare = 99;

	if (isWhiteTurn)
	{
		value = -99999999;
		bestValue = -999999999;

		for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
		{
			makeMove(*it);
			value = alphaBetaSearch(depth - 1, alpha, beta, isWhiteTurn);
			unmakeMove(*it);
			if (value > bestValue)
			{
				bestValue = value;
				bestMove = *it;
			}
		}
	}
	else
	{
		value = 9999999999;
		bestValue = 99999999;
		for (std::list<Move>::iterator it = moveList.begin(); it != moveList.end(); it++)
		{
			makeMove(*it);
			value = alphaBetaSearch(depth - 1, alpha, beta, !isWhiteTurn);
			unmakeMove(*it);

			if (value < bestValue)
			{
				bestValue = value;
				bestMove = *it;
			}
		}
	}

	makeMove(bestMove);
}

void Board::testDiagnolMagics()
{
	U64 occ = ~getEmpty();

	for (int i = 0; i < 63; i++)
	{
		cout << "I: " << i << " results: \n";
		displayU64(diagnolAttacks(occ, i));
	}
}

void Board::testSlidingMagics()
{
	U64 occ = ~getEmpty();

	for (int i = 0; i < 63; i++)
	{
		cout << "I: " << i << " results: \n";
		displayU64(horizontalAttacks(occ, i));
	}
}

Board& Board::operator=(Board& rhs)
{
	for (int i = 0; i < 15; i++)
	{
		pieces[i] = rhs.pieces[i];
	}

	isWhiteTurn = rhs.isWhiteTurn;
	castleRights = rhs.castleRights;
	halfMoveCount = rhs.halfMoveCount;
	fullMoveCount = rhs.fullMoveCount;
	enpassanteIndex = rhs.enpassanteIndex;
	mBishopTbl = &(rhs.mBishopTbl[0]);
	mRookTbl = &(rhs.mRookTbl[0]);
	attackTable = &(rhs.attackTable[0]);
	isOriginal = false;
	capturedPieceLastTurn = rhs.capturedPieceLastTurn;

	boardsSinceCapture.clear();

	for (std::map<string, int>::iterator it = rhs.boardsSinceCapture.begin(); it != rhs.boardsSinceCapture.end(); it++)
	{
		boardsSinceCapture[it->first] = it->second;
	}

	return *this;
}

//void Board::displayDiagnolAttackTable(int squareIndex)
//{
//	cerr << "Square " << squareIndex << " has " << BBits[squareIndex] << " bits" << endl << endl;
//	for (int i = 0; i < (1 << BBits[squareIndex]); i++)
//	{
//		cerr << "I: " << i << endl;
//		displayU64(diagnolAttackTable[squareIndex][i]);
//	}
//}
//
//void Board::displayHorizontalAttackTable(int squareIndex)
//{
//	cerr << "Square " << squareIndex << " has " << RBits[squareIndex] << " bits" << endl << endl;
//	for (int i = 0; i < (1 << RBits[squareIndex]); i++)
//	{
//		cerr << "I: " << i << endl;
//		displayU64(horizontalAttackTable[squareIndex][i]);
//	}
//}

/**********************************
* HELPFULL FUNCTIONS
**********************************/
int cordToIndex(int row, int col)
{
	int result = (row - 1) * 8;
	return result + col;
}

int cordToIndex(int row, char col)
{
	int intCol = tolower(col) - 97;
	return cordToIndex(row, intCol);
}

int cordToIndex(std::string coord)
{
	int index = 0;
	index += (int)tolower(coord[0]) - 97;
	index += 8 * ((int)coord[1] - 49);
	return index;
}


int cordToIndex(char* coord)
{
	int index = 0;
	index += (int)tolower(coord[0]) - 97;
	index += 8 * ((int)coord[1] - 49);
	return index;
}

void displayU64(U64 var)
{
	U64 mask = 1ULL;
	char buffer = 0;
	for (int i = 0; i < 8; i++)
	{
		buffer = 0;
		for (int j = 0; j < 8; j++)
		{
			buffer <<= 1;
			buffer |= (mask & (var >> (j + (7 - i) * 8)));
			
		}
		cout << (bitset<8>)buffer << endl;
	}
	cout << endl;
}

string indexToCoord(int squareIndex)
{
	int col = squareIndex % 8;
	int row = squareIndex / 8;
	string result = "";

	result += (char)(col + 97);
	result += (char)(row + 49);
	
	return result;
}


/*
* Method: COUNT_1S
* Author: Bradley Johnston
*
* Description:
*	Counts the number of 1s in a variable
*/
int count_1s(U64 var)
{
	int r;
	for (r = 0; var; r++, var &= var - 1);
	return r;
}