// SDchess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Square.h"
#include "Board.h"
#include "Move.h"
#include "test.h"

int moveGenerationTest(chess::Board& board, int depth) {
    if (depth == 0) {
        return 1;
    }

    int numPositions =0;
    bool printed = false;
    auto moves = board.getAllLegalMoves();
    for (chess::Move move : moves) {
        board.makeMove(move);
        numPositions += moveGenerationTest(board, depth - 1);
        board.unmakeMove();
    }

    return numPositions;
}
/*int perft_recursive(chess::Board& board, int depth) {
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


            std::cout << move.getMoveText() << ": " << numPositions << std::endl;
            totalNodes += numPositions;
        }
        
        std::cout << "-----------" << std::endl;
        std::cout << "Total nodes" << ": " << totalNodes << std::endl;
        return totalNodes;
    }*/
int main()
{

    std::string fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    //std::string fen = "3k2rr/8/8/8/8/8/8/4K3 w - - 0 1";
   // std::string fen = "5k1r/8/8/8/8/8/4P3/4K2R w K - 0 1";
   // std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
   auto board = chess::Board(fen);

    //while (true) {
    //    auto kada = testiram();
    //  //  std::cout << kada.first<<std::endl;
    //    auto board = chess::Board(kada.first);
    //    int numPositions = moveGenerationTest(board, 1);
    //    if (kada.second != numPositions) {
    //        std::cout << kada.first << ", numOfMoves: " <<numPositions <<std::endl;
    //        return 0;
    //    }
    //}
     
    //board.printBoard();
    //board.isPinned();
    //std::cout << std::endl << board.isCheck()<< std::endl;
    //auto moves = board.generatePieceMoves(chess::Position("E8"));
    //auto moves = board.getAllLegalMoves();
    
  int numPositions = moveGenerationTest(board, 5);
   std::cout << numPositions << std::endl;
  
   auto move = chess::Move("H1","H8",false,false,chess::PieceType::None);
   board.makeMove(move);
    move = chess::Move("F8","E7",false,false,chess::PieceType::None);
   board.makeMove(move);
   
    auto moves = board.getAllLegalMoves();
    std::cout << moves.size() << std::endl;
    for (chess::Move move : moves) {

        //board.makeMove(move);
        board.makeMove(move);
        auto tada = board.getAllLegalMoves();
        std::cout << move.getMoveText() <<", "<< tada.size() << std::endl; //<<" - NumOfMoves: " << board.getAllLegalMoves().size() << std::endl;
        board.unmakeMove();
      // board.unmakeMove();

    }

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
