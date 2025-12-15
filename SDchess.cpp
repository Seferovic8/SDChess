// SDchess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WIN32_LEAN_AND_MEAN // <--- Fixes the conflict

#include <winsock2.h>       // <--- Include specific winsock headers first
#include <ws2tcpip.h>
#include <iostream>
#include "Square/Square.h"
#include "Board/Board.h"
#include "Move/Move.h"
#include "test.h"
#include "server.h"
#include <chrono>

//int moveGenerationTest(chess::Board& board, int depth) {
//    if (depth == 0) {
//        return 1;
//    }
//
//    int numPositions =0;
//    bool printed = false;
//    auto moves = board.getAllLegalMoves();
//    for (chess::Move move : moves) {
//        board.makeMove(move);
//        numPositions += moveGenerationTest(board, depth - 1);
//        board.unmakeMove();
//    }
//
//    return numPositions;
//}
int perft_recursive(chess::Board& board, int depth) {
	if (depth == 0) {
		return 1;
	}

	int numPositions = 0;
	bool printed = false;
	auto moves = board.getAllLegalMoves();
	for (chess::Move move : moves) {
		board.makeMove(move);
		numPositions += perft_recursive(board, depth - 1);
		board.unmakeMove();
		//std::cout << move.getMoveText() << ": " << numPositions << std::endl;

	}

	return numPositions;
}
int moveGenerationTest(chess::Board& board, int depth) {
	int totalNodes = 0;
	auto moves = board.getAllLegalMoves();
	for (chess::Move move : moves) {

		board.makeMove(move);
		int numPositions = perft_recursive(board, depth - 1);
		board.unmakeMove();


		//std::cout << std::endl;
		std::cout << move.getMoveText() << ": " << numPositions << std::endl;
		//std::cout << std::endl << std::endl << std::endl;
		totalNodes += numPositions;
	}

	std::cout << "-----------" << std::endl;
	std::cout << "Total nodes" << ": " << totalNodes << std::endl;
	return totalNodes;
}
int main()
{
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	//std::string fen = "3k2rr/8/8/8/8/8/8/4K3 w - - 0 1";
   // std::string fen = "5k1r/8/8/8/8/8/4P3/4K2R w K - 0 1";
	//std::string fen = "3k4/8/8/8/8/8/8/3K3R w - - 1 1";

	auto board = chess::Board(fen);
	//std::cout << board.evaluate();
	//server();
	//auto mv = chess::Move("D2", "D4");
	//	board.makeMove(mv);
	//	board.printBoard();
	//	return 1;
	



	//while (true) {
	//    auto kada = testiram();
	//  //  std::cout << kada.first<<std::endl;
	//    auto board = chess::Board(kada.first);
	//    if (kada.second != numPositions) {
	//        std::cout << kada.first << ", numOfMoves: " <<numPositions <<std::endl;
	//        return 0;
	//    }
	//}

	//board.printBoard();
	//board.isPinned();
	//auto moves = board.generatePieceMoves(chess::Position("E8"));
		//auto move = chess::Move("D1","D2",false,false,chess::PieceType::None);
	//board.makeMove(move);
	/*move = chess::Move("D8", "E8", false, false, chess::PieceType::None);
	board.makeMove(move);*/
	//board.unmakeMove();
	//move = chess::Move("E1", "G1", false, true, chess::PieceType::None);
	//board.makeMove(move);
	//board.unmakeMove();
	auto moves = board.getAllLegalMoves();
	board.printBoard();
	auto t1 = high_resolution_clock::now();
	    int numPositions = moveGenerationTest(board, 6);
	std::cout << numPositions << std::endl;
   //int numPositions = moveGenerationTest(board, 5);
   //std::cout<< numPositions << std::endl; //<<" - NumOfMoves: " << board.getAllLegalMoves().size() << std::endl;
	auto t2 = high_resolution_clock::now();

	/*for (int i = 0; i < 15; i++) {
		auto move = board.findBestMove(5);
		std::cout << move.getMoveText() << std::endl;
		board.makeMove(move);
		auto k = testiram(chess::Position::positionToNum( move.getFromPos()), chess::Position::positionToNum(move.getToPos()));
		std::cout << k.first << std::endl;
		board = chess::Board(k.first);
		if (board.isCheckMate()) {
			break;
		}
	}*/
	board.printBoard();
	auto ms_int = duration_cast<milliseconds>(t2 - t1);

	/* Getting number of milliseconds as a double. */
	duration<double, std::milli> ms_double = t2 - t1;

	std::cout << ms_int.count() << "ms\n";
	std::cout << ms_double.count() << "ms\n";
	//move = chess::Move("A7","A5",false,false,chess::PieceType::None);
	//board.makeMove(move);
	//move = chess::Move("B4", "B5", false, false, chess::PieceType::None);
	//board.makeMove(move);
	//move = chess::Move("A8", "A6", false, false, chess::PieceType::None);
	//board.makeMove(move);
	//
	//auto moves = board.getAllLegalMoves();
	// std::cout << moves.size() << std::endl;
	 //for (chess::Move move : moves) {

	 //    //board.makeMove(move);
	 //    board.makeMove(move);
	 //    auto tada = board.getAllLegalMoves();
	 //    std::cout << move.getMoveText() <<", "<< tada.count << std::endl; //<<" - NumOfMoves: " << board.getAllLegalMoves().size() << std::endl;
	 //    board.unmakeMove();
	 //  // board.unmakeMove();

	 //}

	// std::cout << moves.size() << std::endl;
	 //auto move = moves.front();
	// board.makeMove(move);
	// move = chess::Move("B5","C6",true,false,chess::PieceType::None);
	// board.printBoard();
	// board.makeMove(move);
	// std::cout<<std::endl;
	// board.printBoard();
	// std::cout<<std::endl;
	// board.unmakeMove();
	//// move = chess::Move("C4", "D3");
	// //board.makeMove(move);
	// board.printBoard();
	//// std::cout << move.getMoveText() << std::endl;
	//  auto moves = board.getAllLegalMoves();
	//// board.printBoard();
	// std::string k = (board.sideToMove == chess::Color::White) ? "white" : "black";
	// std::cout << std::endl << moves.size() << std::endl;
	// for (auto move : moves) {
	//     std::cout << move.getMoveText() << std::endl;
	// }
	 //board.makeMove(move);
	 //board.printBoard();
	 //std::cout<<std::endl;
	 //board.unmakeMove();
	 //board.printBoard();
}
