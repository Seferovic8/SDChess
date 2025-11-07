#pragma once
#include<iostream>
#include<string>
#include "Square.h"
#include "Bitboard.h"
#include <array>   

#include "Move.h"
#include <vector>
namespace chess {
	using VectorBoard = std::vector<std::vector<Square>>;

	struct CastlingRights {
		bool whiteKing = true; // K
		bool whiteQueen = true; // Q
		bool blackKing = true; // k
		bool blackQueen = true; // q
	};
	class Board {
	private:
		VectorBoard board;
		Bitboard bitboard;
		std::vector<Move> history;
		Color sideToMove;
		CastlingRights castling;
	public:
		Board();
		Board(std::string fen);
		void printBoard() const;
		std::vector<chess::Move> generatePieceMoves(chess::Position fromPos);
		std::vector<chess::Move> getAllPseudoLegalMoves();
		std::vector<chess::Move> getAllLegalMoves();
		bool isInside(int r, int c);
		void checkBitBoards();
		bool isCheck();
		bool isPinned();
	};
}