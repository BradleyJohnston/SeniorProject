// ChessEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define RANDOM_MOVE_COUNT 6
#define MAX_THREAD_COUNT 10
#define GAME_COUNT 25
#define THREAD_DEPTH 5

#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <map>
#include <Windows.h>
#include "Board.h"
#include "NeatChessEngine.h"
#include "MonteCarloSearchTree.h"

using namespace std;


static map<string, int> results;
static HANDLE semaphore;

void testBoardDisplay(Board & board);
void testBoardLoad(Board & board);
void testBoardSave(Board & board);
void testPawnMovement(Board & board);
void testPawnEnpassant(Board & board);
void testKnightMovement(Board & board);
void testBishopMovement(Board & board);
void testRookMovement(Board & board);
void testQueenMovement(Board & board);
void testKingMovement(Board & board);
void testKingChecked(Board & board);
void test3foldRepitition(Board& board);
void testBasicMoveGeneration(Board & board);
void testAdvancedMoveGeneration(Board & board);
void testEngineGameLoop();
void testEngineGameLoopSolo();
void testEngineAutomatedGame();
string testEngineAutomatedGame(string FEN);
string testEngineAutomatedGame(NeatChessEngine engine);
void saveMonteCarloToFile();

DWORD WINAPI threadedTestEngineAutomated(void* threadData);

void dynamicPerftTesting(Board & board);

void specificTesting(Board& board);

void GameLoop(Board& board);
void evaluatedGameLoop(Board& board);
void autoPlayGame(Board& board);

void incrementResults(string type);

void MonteCarloTesting();

//void saveAttackTable(string fileName, Board board, int BishopInices[], int RookIndices[]);
//void saveBishopMagic(string fileName, Board board, int BishopIndices[]);
//void saveRookMagic(string fileName, Board board, int RookIndeces[]);


int main()
{
     
    Board board;
    GameLoop(board);
    return 0;
}

void testBoardDisplay(Board & board)
{

    return;
}

void testBoardLoad(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    //// Declare variables
    cout << "Board before load" << endl;
    board.displayConsole();
    board.loadBoard("k7/8/3p1p2/8/2P3P1/3PPP2/8/K7 b - - 0 1");
    cout << "Board after load" << endl;
    board.displayConsole();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    cout << "\nRan Test Board Load in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testBoardSave(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    // Declare variables
    board.loadBoard("rn2k2r/pp2bppp/8/1B1p4/2PP1B2/7P/PP3PP1/R4RK1 b kq c3 0 1");
    board.displayConsole();
    string FENcode = board.saveBoard();
    cout << FENcode << endl;
    cout << "rn2k2r/pp2bppp/8/1B1p4/2PP1B2/7P/PP3PP1/R4RK1 b kq c3 0 1" << endl;
    cout << FENcode.compare("rn2k2r/pp2bppp/8/1B1p4/2PP1B2/7P/PP3PP1/R4RK1 b kq c3 0 1") << endl;

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    cout << "\nRan Test Board Display in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testPawnMovement(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    // Declare variables
    board.loadBoard("k7/p1p1p1p1/3P3P/8/8/3p3p/P1P1P1P1/K7 w - - 0 1");
    cout << "Board before tests" << endl;
    board.displayConsole();
    board.movePiece(8, 16);
    board.movePiece(48, 40);
    board.movePiece(10, 26);
    board.movePiece(50, 34);
    board.movePiece(12, 19);
    board.movePiece(52, 43);
    board.movePiece(14, 23);
    board.movePiece(54, 47);
    cout << "\nBoard after moves" << endl;
    board.displayConsole();
    cout << "\nBoard should look like this" << endl;
    board.loadBoard("k7/8/p2p3p/2p5/2P5/P2P3P/8/K7 w - - 0 1");
    board.displayConsole();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    cout << "\nRan Test Board Pawn Movement in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testPawnEnpassant(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    // Declare variables
    board.loadBoard("k7/1p3p2/8/2P1P3/2p1p3/8/1P3P2/K7 w - - 0 1");
    board.displayConsole();

    board.movePiece(9, 25);
    board.movePiece(26, 17);

    board.movePiece(13, 29);
    board.movePiece(28, 21);

    board.movePiece(0, 1);

    board.movePiece(49, 33);
    board.movePiece(34, 41);

    board.movePiece(53, 37);
    board.movePiece(36, 45);

    board.displayConsole();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    cout << "\nRan Pawn Enpassant Display in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testKnightMovement(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    // Declare variables
    board.loadBoard("k7/2nnnn2/2nnnn2/8/8/2NNNN2/2NNNN2/K7 w - - 0 1");
    cout << "Board before tests" << endl;
    board.displayConsole();

    cout << "\nBoard after tests" << endl;

    board.movePiece(10, 27);
    board.movePiece(42, 59);
    board.movePiece(11, 17);
    board.movePiece(43, 49);
    board.movePiece(12, 22);
    board.movePiece(44, 54);
    board.movePiece(13, 28);
    board.movePiece(45, 60);
    board.movePiece(18, 3);
    board.movePiece(50, 35);
    board.movePiece(19, 9);
    board.movePiece(51, 41);
    board.movePiece(20, 14);
    board.movePiece(52, 46);
    board.movePiece(21, 4);
    board.movePiece(53, 36);

    board.displayConsole();
    
    cout << "\nBoard should look like this" << endl;
    board.loadBoard("k2nn3/1n4n1/1n4n1/3nn3/3NN3/1N4N1/1N4N1/K2NN3 w - - 0 1");
    board.displayConsole();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    cout << "\nRan Test Board Knight Movement in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testBishopMovement(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    // Declare variables
    board.loadBoard("kr6/pp1bb3/8/8/8/8/PP1BB3/KR6 w - - 0 1");

    cout << "Board before tests" << endl;
    board.displayConsole();
    cout << "\nRunning single movement tests" << endl;

    board.movePiece(11, 20);
    board.movePiece(51, 44);
    board.movePiece(12, 19);
    board.movePiece(52, 43);

    board.displayConsole();
    cout << "\nShould look like this" << endl;
    board.loadBoard("kr6/pp6/3bb3/8/8/3BB3/PP6/KR6 w - - 0 1");
    board.displayConsole();

    cout << "\nTesting multiple square moves" << endl;
    board.movePiece(19, 55);
    board.movePiece(43, 15);
    board.movePiece(20, 48);
    board.movePiece(44, 8);
    board.displayConsole();

    cout << "\nBoard should look like this" << endl;
    board.loadBoard("kr6/Bp5B/8/8/8/8/bP5b/KR6 w - - 0 1");
    board.displayConsole();


    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    cout << "\nRan Test Bishop Movements in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testRookMovement(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    // Declare variables
    cout << "Board before tests" << endl;
    board.loadBoard("kr6/pp6/6r1/8/8/5R2/PP6/KR6 w - - 0 1");
    board.displayConsole();

    cout << "Board after tests" << endl;
    
    // White moves
    board.movePiece(21, 19);
    board.movePiece(46, 30);
    board.movePiece(19, 35);
    board.movePiece(30, 27);
    board.movePiece(35, 37);
    board.movePiece(27, 43);
    board.movePiece(37, 5);
    board.movePiece(43, 47);

    board.displayConsole();

    cout << "\nBoard should look like" << endl;
    board.loadBoard("kr6/pp6/7r/8/8/8/PP6/KR3R2 w - - 0 1");
    board.displayConsole();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    cout << "\nRan Test Rook Movements in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testQueenMovement(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    // Declare variables
    board.loadBoard("kr6/pp6/5q2/8/8/5Q2/PP6/KR6 w - - 0 1");

    // White horizontal moves
    board.movePiece(21, 19);
    board.movePiece(45, 29);
    board.movePiece(19, 35);
    board.movePiece(29, 27);
    board.movePiece(35, 37);
    board.movePiece(27, 43);
    board.movePiece(37, 21);
    board.movePiece(43, 45);

    // White diagnol moves
    board.movePiece(21, 35);
    board.movePiece(45, 27);
    board.movePiece(35, 17);
    board.movePiece(27, 41);
    board.movePiece(17, 3);
    board.movePiece(41, 59);
    board.movePiece(3, 30);
    board.movePiece(59, 38);


    board.displayConsole();

    cout << "\nBoard should look like" << endl;
    board.loadBoard("kr6/pp6/8/6q1/6Q1/8/PP6/KR6 w - - 0 1");
    board.displayConsole();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    cout << "\nRan Test Queen Movements in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testKingMovement(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    // Declare variables
    board.loadBoard("r3k2r/ppp2ppp/3pp3/8/8/3PP3/PPP2PPP/R3K2R w KQkq - 0 1");

    cout << "Board before tests" << endl;
    board.displayConsole();

    cout << "\nKingside Castling" << endl;
    board.movePiece(4, 6);
    board.movePiece(60, 62);
    board.displayConsole();

    cout << "\nBoard should look like" << endl;
    board.loadBoard("r4rk1/ppp2ppp/3pp3/8/8/3PP3/PPP2PPP/R4RK1 w - - 0 1");
    board.displayConsole();

    cout << "\nQueenside Castling" << endl;
    board.loadBoard("r3k2r/ppp2ppp/3pp3/8/8/3PP3/PPP2PPP/R3K2R w KQkq - 0 1");
    board.movePiece(4, 2);
    board.movePiece(60, 58);
    board.displayConsole();

    cout << "\nBoard should look like" << endl;
    board.loadBoard("2kr3r/ppp2ppp/3pp3/8/8/3PP3/PPP2PPP/2KR3R w - - 0 1");
    board.displayConsole();

    cout << "General King movement" << endl;
    board.loadBoard("r3k2r/ppp2ppp/3pp3/8/8/3PP3/PPP2PPP/R3K2R w KQkq - 0 1");

    // White king
    board.movePiece(4, 12);
    board.movePiece(60, 59);
    board.movePiece(12, 21);
    board.movePiece(59, 51);
    board.movePiece(21, 28);
    board.movePiece(51, 52);
    board.movePiece(28, 27);
    board.movePiece(52, 45);
    board.movePiece(27, 18);
    board.movePiece(45, 36);
    board.movePiece(18, 11);
    board.movePiece(36, 35);
    board.movePiece(11, 3);
    board.movePiece(35, 42);
    board.movePiece(3, 2);
    board.movePiece(42, 51);
    board.movePiece(2, 3);
    board.movePiece(51, 59);

    board.displayConsole();

    cout << "\nBoard should look like" << endl;
    board.loadBoard("r2k3r/ppp2ppp/3pp3/8/8/3PP3/PPP2PPP/R2K3R w - - 0 1");

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    cout << "\nRan Test King Movement in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testKingChecked(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    // Declare variables
    cout << "Board starting with" << endl;
    board.loadBoard("rnb1kbnr/ppp2ppp/8/4q3/8/2N5/PPPP1PPP/R1BQKB1R w KQkq - 0 1");
    board.displayConsole();
    board.movePiece(4, 12);
    cout << "\nBoard should not have changed" << endl;
    board.displayConsole();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    cout << "\nRan Test King Checked in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void test3foldRepitition(Board& board)
{
    board.loadBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    for (int i = 0; i < 10; i++)
    {
        cout << "Attempting iteration " << i << endl;
        cout << "Returned count: " << board.getStateCount() << endl;
        board.movePiece(1, 18);
        board.movePiece(57, 42);
        board.movePiece(18, 1);
        board.movePiece(42, 57);
    }
}

void testBasicMoveGeneration(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    // Declare variables
    board.loadBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    board.displayConsole();
    int positionCount = board.getMoveCount(board, 2);
    cout << "Should provide 20 legal moves. Actual was " << positionCount << endl;

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    cout << "\nRan Test Basic Move Generation in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testAdvancedMoveGeneration(Board & board)
{
    auto begin = std::chrono::high_resolution_clock::now();

    bool failureFlag = false;

    string FEN1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    string FEN2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0";
    string FEN3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0";
    string FEN4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    string FEN5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    string FEN6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

    int perf1[5] = { 20, 400, 8902, 197281, 4865609 };
    int perf2[5] = { 48, 2039, 97862, 4085603, 193690690};
    int perf3[5] = { 14, 191, 2812, 43238, 674624};
    int perf4[5] = { 6, 264, 9467, 422333, 15833292};
    int perf5[5] = { 44, 1486, 62379, 2103487, 89941194};
    int perf6[5] = { 46, 2079, 89890, 3894594, 164075551 };

    // Declare variables
    cout << board.saveBoard() << endl;

    auto depthBegin = std::chrono::high_resolution_clock::now();
    auto depthEnd = std::chrono::high_resolution_clock::now();
    auto depthElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(depthEnd - depthBegin);
    int moveCount;

    cout << "Perft 1" << endl;
    board.displayConsole();
    for (int i = 1; i < 5; i++)
    {
        depthBegin = std::chrono::high_resolution_clock::now();
        moveCount = board.getMoveCount(board, i, false);
        cout << "Depth: " << i << " provided " << moveCount << "/" << perf1[i-1] << " moves." 
             << " Giving us " << perf1[i-1] - moveCount << " missing moves." << endl;
        if (perf1[i - 1] - moveCount != 0)
        {
            failureFlag = true;
        }
        depthEnd = std::chrono::high_resolution_clock::now();
        depthElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(depthEnd - depthBegin);
        cout << i << " depth search took " << (depthElapsed.count() * 1e-9) << " seconds." << endl << endl;
    }
    board.displayConsole();

    cout << "\n\n\nPerft2" << endl;
    board.loadBoard(FEN2);
    board.displayConsole();
    for (int i = 1; i < 5; i++)
    {
        depthBegin = std::chrono::high_resolution_clock::now();
        moveCount = board.getMoveCount(board, i, false);
        cout << "Depth: " << i << " provided " << moveCount << "/" << perf2[i - 1] << " moves."
            << " Giving us " << perf2[i - 1] - moveCount << " missing moves." << endl;
        if (perf2[i - 1] - moveCount != 0)
        {
            failureFlag = true;
        }
        depthEnd = std::chrono::high_resolution_clock::now();
        depthElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(depthEnd - depthBegin);
        cout << i << " depth search took " << (depthElapsed.count() * 1e-9) << " seconds." << endl << endl;
    }
    board.displayConsole();

    cout << "\n\n\nPerft3" << endl;
    board.loadBoard(FEN3);
    board.displayConsole();
    for (int i = 1; i < 5; i++)
    {
        depthBegin = std::chrono::high_resolution_clock::now();
        moveCount = board.getMoveCount(board, i, false);
        cout << "Depth: " << i << " provided " << moveCount << "/" << perf3[i - 1] << " moves."
            << " Giving us " << perf3[i - 1] - moveCount << " missing moves." << endl;
        if (perf3[i - 1] - moveCount != 0)
        {
            failureFlag = true;
        }
        depthEnd = std::chrono::high_resolution_clock::now();
        depthElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(depthEnd - depthBegin);
        cout << i << " depth search took " << (depthElapsed.count() * 1e-9) << " seconds." << endl << endl;
    }
    board.displayConsole();

    cout << "\n\n\nPerft4" << endl;
    board.loadBoard(FEN4);
    board.displayConsole();
    for (int i = 1; i < 5; i++)
    {
        depthBegin = std::chrono::high_resolution_clock::now();
        moveCount = board.getMoveCount(board, i, false);
        cout << "Depth: " << i << " provided " << moveCount << "/" << perf4[i - 1] << " moves."
            << " Giving us " << perf4[i - 1] - moveCount << " missing moves." << endl;
        if (perf4[i - 1] - moveCount != 0)
        {
            failureFlag = true;
        }
        depthEnd = std::chrono::high_resolution_clock::now();
        depthElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(depthEnd - depthBegin);
        cout << i << " depth search took " << (depthElapsed.count() * 1e-9) << " seconds." << endl << endl;
    }
    board.displayConsole();

    cout << "\n\n\nPerft5" << endl;
    board.loadBoard(FEN5);
    board.displayConsole();
    for (int i = 1; i < 5; i++)
    {
        depthBegin = std::chrono::high_resolution_clock::now();
        moveCount = board.getMoveCount(board, i, false);
        cout << "Depth: " << i << " provided " << moveCount << "/" << perf5[i - 1] << " moves."
            << " Giving us " << perf5[i - 1] - moveCount << " missing moves." << endl;
        if (perf5[i - 1] - moveCount != 0)
        {
            failureFlag = true;
        }
        depthEnd = std::chrono::high_resolution_clock::now();
        depthElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(depthEnd - depthBegin);
        cout << i << " depth search took " << (depthElapsed.count() * 1e-9) << " seconds." << endl << endl;
    }
    board.displayConsole();

    cout << "\n\n\nPerft6" << endl;
    board.loadBoard(FEN6);
    board.displayConsole();
    for (int i = 1; i < 5; i++)
    {
        depthBegin = std::chrono::high_resolution_clock::now();
        moveCount = board.getMoveCount(board, i, false);
        cout << "Depth: " << i << " provided " << moveCount << "/" << perf6[i - 1] << " moves."
            << " Giving us " << perf6[i - 1] - moveCount << " missing moves." << endl;
        if (perf6[i - 1] - moveCount != 0)
        {
            failureFlag = true;
        }
        depthEnd = std::chrono::high_resolution_clock::now();
        depthElapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(depthEnd - depthBegin);
        cout << i << " depth search took " << (depthElapsed.count() * 1e-9) << " seconds." << endl << endl;
    }
    board.displayConsole();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    if (failureFlag)
    {
        cerr << "Test(s) Failed" << endl;
    }
    else
    {
        cerr << "Tests Passed!" << endl;
    }

    cout << "\nRan Test Advanced Move Generation in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
    return;
}

void testEngineGameLoop()
{
    cerr << "Entering Engine Game Loop" << endl;
    NeatChessEngine engine;
    engine.GameLoop(rand() % 2 == 0);
    engine.displayBoard();
}

void testEngineGameLoopSolo()
{
    cerr << "Entering Solo Engine Game Loop" << endl;
    NeatChessEngine engine;
    engine.SoloGameLoop();
    engine.displayBoard();
}

void testEngineAutomatedGame()
{
    auto begin = std::chrono::high_resolution_clock::now();

    NeatChessEngine engine;
    engine.automatedGameLoop();
    engine.displayBoard();


    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    cout << "\nRan Test Board Display in " << (elapsed.count() * 1e-9) << " seconds" << endl << endl;
}

string testEngineAutomatedGame(string FEN)
{
    NeatChessEngine engine;
    engine.loadFEN(FEN);
    engine.automatedGameLoop();
    engine.displayBoard();

    list<Move> gameMoves = engine.getGameHistory();

    for (std::list<Move>::iterator it = gameMoves.begin(); it != gameMoves.end(); it++)
    {
        cout << indexToCoord(it->startingSquare) << indexToCoord(it->targetSquare) << endl;
    }

    return engine.getResult();
}

string testEngineAutomatedGame(NeatChessEngine engine)
{
    engine.displayBoard();
    engine.automatedGameLoop();
    engine.displayBoard();

    list<Move> gameMoves = engine.getGameHistory();

    for (std::list<Move>::iterator it = gameMoves.begin(); it != gameMoves.end(); it++)
    {
        cout << indexToCoord(it->startingSquare) << indexToCoord(it->targetSquare) << endl;
    }

    return engine.getResult();
}

void saveMonteCarloToFile()
{
    Board board;
    MonteCarloSearchTree tree(board);
    ifstream fin("coordFile.txt");
    string buffer = "";
    resultCount results;
    results.wins = 0;
    results.draws = 0;
    results.losses = 0;

    while (getline(fin, buffer))
    {
        // If it is an empty line
        if (buffer.size() < 5)
        {
            continue;
        }

        if (buffer[0] == '-')
        {
            results.wins = 0;
            results.draws = 0;
            results.losses = 1;

            buffer.erase(0, 3);
        }
        else if (buffer[0] == '1')
        {
            results.wins = 1;
            results.losses = 0;
            results.draws = 0;
            
            buffer.erase(0, 2);
        }
        else if (buffer[0] == '0')
        {
            results.wins = 0;
            results.draws = 1;
            results.losses = 0;

            buffer.erase(0, 2);
        }
        try
        {
            cout << "Running game " << buffer << endl;
            tree.runGame(results, buffer);
        }
        catch (exception e)
        {
            continue;
        }
    }

    tree.saveToFile("MonteCarloResults.txt");
}

DWORD WINAPI threadedTestEngineAutomated(void* threadData)
{
    float* movementWeight = (float*)threadData;

    NeatChessEngine engine;
    engine.setMovementWeight(*movementWeight);

    
    for (int i = 0; i < GAME_COUNT; i++)
    {
        engine.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        for (int j = 0; j < RANDOM_MOVE_COUNT; j++)
        {
            engine.makeMove(engine.GenerateRandomMove());
        }
        engine.automatedGameLoop();
        string result = engine.getResult();
        if (result != THREEFOLD_REPITION)
        {
            cout << "Movement: " << *movementWeight << " resulted in " << result << endl;
        }
        incrementResults(result);
    }

    cout << "thread finished" << endl;

    return 0;
}

void dynamicPerftTesting(Board & board)
{
    string FENcode = "";
    string previousFEN = "";
    int depth = 0;

    cout << "Enter a FEN code: ";
    getline(cin, FENcode);

    while (FENcode != "r" || FENcode.size() < 10)
    {
        if (FENcode == "r")
        {
            FENcode = previousFEN;
        }

        board.loadBoard(FENcode);

        cout << "Enter a depth: ";
        cin >> depth;

        cout << board.getMoveCount(board, depth, true) << " total moves" << endl;

        board.displayConsole();

        previousFEN = FENcode;

        cout << "Enter a FEN code: ";
        cin.ignore();
        getline(cin, FENcode);
    }
}

void specificTesting(Board& board)
{
    cout << "Loading" << endl;
    board.loadBoard("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    board.movePiece(35, 43);
    board.displayConsole();
    cout << board.saveBoard() << endl;
    cout << board.getMoveCount(board, 1, true) << " moves" << endl;
    board.displayConsole();

    board.movePiece(44, 35);
    board.displayConsole();

}

void GameLoop(Board& board)
{
    board.loadBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    string coord = "";
    int index = 0;
    while (board.getMoveCount(board, 1) > 0)
    {
        // White Turn
        if (board.isWhite())
        {
            board.displayConsole();
            cout << "Enter starting square: ";
            cin >> coord;
            index = cordToIndex(coord);
            cerr << "Index: " << index << endl;
            cout << "Enter target Square: ";
            cin >> coord;
            board.movePiece(index, cordToIndex(coord));
        }
        // Black Turn
        else
        {
            list<Move> moves = board.generateMoves();
            int index = rand() % moves.size();
            for (int i = 0; i < index; i++)
            {
                moves.pop_front();
            }
            board.makeMove(moves.front());
        }
    }
}

void evaluatedGameLoop(Board& board)
{
    board.loadBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    string coord = "";
    int index = 0;
    int moveCount = 0;
    while (board.getMoveCount(board, 1) > 0)
    {
        moveCount += 1;
        // White Turn
        if (board.isWhite())
        {
            board.displayConsole();
            cout << "Enter starting square: ";
            cin >> coord;
            index = cordToIndex(coord);
            cerr << "Index: " << index << endl;
            cout << "Enter target Square: ";
            cin >> coord;
            board.movePiece(index, cordToIndex(coord));
        }
        // Black Turn
        else
        {
            board.GenerateBestMove();
        }
    }

    cerr << "Game over in " << moveCount << " moves" << endl;
}

void autoPlayGame(Board& board)
{

    board.loadBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    string coord = "";
    int index = 0;
    int moveCount = 0;
    while (board.getMoveCount(board, 1) > 0)
    {
        if (moveCount > 500)
        {
            break;
        }
        board.displayConsole();
        moveCount += 1;
        // White Turn
        if (board.isWhite())
        {
            board.GenerateBestMove(6);
        }
        // Black Turn
        else
        {
            board.GenerateBestMove(6);
        }
    }
    
    cerr << "Game over in " << moveCount << " moves" << endl;

}

void incrementResults(string type)
{
    WaitForSingleObject(semaphore, INFINITE);
    results[type] += 1;
    ReleaseSemaphore(semaphore, 1, 0);
}

void MonteCarloTesting()
{

    cout << "Running Monte Carlo Test" << endl;
    Board board;
    MonteCarloSearchTree mcsTree(board);


    resultCount results;

    results.wins = 0;
    results.draws = 0;
    results.losses = 1;
    cout << "Before running game" << endl;
    mcsTree.runGame(results, "g1f3 c7c6 g2g3 d7d5 ");
    cout << "After running game\nBefore saving to file" << endl;
    
    mcsTree.saveToFile("MonteCarlo.txt");
}

//void saveAttackTable(string fileName, Board & board, int BishopInices[], int RookIndices[])
//{
//    int attackTableSize = 0;
//
//    U64** diagnolAttackTable = board.getDiagnolAttackTable();
//    U64** horizontalAttackTable = board.getHorizontalAttackTable();
//    int bitShift = 0;
//
//    for (int i = 0; i < 64; i++)
//    {
//        bitShift = board.getRBits(i);
//        cout << "Rook: " << i << "; " << bitShift << " bits" << endl;
//        for (int j = 0; j < (1 << (bitShift)); j++)
//        {
//            if (horizontalAttackTable[i][j])
//            {
//                attackTableSize += 1;
//            }
//        }
//        bitShift = board.getBBits(i);
//        cout << "Bishop: " << bitShift << " bits" << endl;
//        for (int j = 0; j < (1 << (bitShift)); j++)
//        {
//            if (diagnolAttackTable[i][j])   
//                attackTableSize += 1;
//        }
//        
//    }
//
//    ofstream fout(fileName.c_str());
//
//    fout << attackTableSize << endl;
//    attackTableSize = 0;
//    for (int i = 0; i < 64; i++)
//    {
//        BishopInices[i] = attackTableSize;
//        cerr << "Bishop Index " << i << ": " << attackTableSize << endl;
//        for (int j = 0; j < (1 << board.getBBits(i)); j++)
//        {
//            if (diagnolAttackTable[i][j] != 0ULL)
//            {
//                fout << diagnolAttackTable[i][j] << "\n";
//                attackTableSize += 1;
//            }
//        }
//    }
//
//    for (int i = 0; i < 64; i++)
//    {
//        RookIndices[i] = attackTableSize;
//        cerr << "Rook Index " << i << ": " << attackTableSize << endl;
//        for (int j = 0; j < (1 << board.getRBits(i)); j++)
//        {
//            if (horizontalAttackTable[i][j] != 0ULL)
//            {
//                fout << horizontalAttackTable[i][j] << "\n";
//                attackTableSize += 1;
//            }
//        }
//    }
//
//    fout.close();
//}

/*
    U64* ptr;  // Pointer to attack_table
    U64 mask;  // to mask relevant squares of both lines (no outer squares)
    U64 magic; // Magic 64-bit factor
    int shift; // Shift right
*/

void saveBishopMagic(string fileName, Board & board, int BishopIndices[])
{
    SMagic* mBishop = board.getMBishopTable();
    ofstream fout(fileName.c_str());

    for (int i = 0; i < 64; i++)
    {
        // AttackTable Index
        fout << BishopIndices[i] << " ";
        // Mask
        fout << mBishop[i].mask << " ";
        // Magic
        fout << mBishop[i].magic << " ";
        // Shift
        fout << mBishop[i].shift << "\n";
    }

    fout.close();
}

void saveRookMagic(string fileName, Board & board, int RookIndices[])
{
    SMagic* mRook = board.getMRookTable();
    ofstream fout(fileName.c_str());

    for (int i = 0; i < 64; i++)
    {
        // Attack Table Index
        fout << RookIndices[i] << " ";
        // Mask
        fout << mRook[i].mask << " ";
        // Magic
        fout << mRook[i].magic << " ";
        // Shift
        fout << mRook[i].shift << "\n";
    }

    fout.close();
}
