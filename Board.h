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

		void castlingFinished() {
			whiteKing = false; // K
			whiteQueen = false; // Q
			blackKing = false; // k
			blackQueen = false; // q
		}
	};
	struct GameState {
		Move move;
		CastlingRights previousCastlingRights;
		Color moveColor;
		chess::PieceType playedPiece;
		bool isCapture;
		bool enPassant;
		bool isPromotion;
		bool isCastling;
		chess::PieceType capturedPiece;
		// 3. Hash (Optional)
		//uint64_t previousZobristKey;
		GameState() = default;
		GameState(Move m,chess::PieceType piece,Color color, CastlingRights cr, bool cap = false, bool en = false, bool prom = false,bool cast=false, chess::PieceType pt = chess::PieceType::None)
			: move(m), playedPiece(piece), moveColor(color), previousCastlingRights(cr), isCapture(cap), enPassant(en), isPromotion(prom), isCastling(cast), capturedPiece(pt)
		{}
	};
	class Board {
	private:
		VectorBoard board;
		Bitboard bitboard;
		std::vector<GameState> history;
		CastlingRights castling;
	public:
		Color sideToMove;
		Board();
		Board(std::string fen);
		void printBoard() const;
		std::vector<chess::Move> generatePieceMoves(chess::Position fromPos);
		std::vector<chess::Move> getAllPseudoLegalMoves();
		std::vector<chess::Move> getAllLegalMoves();
		bool isInside(int r, int c);
		void rookCastling(chess::Position pos, Color pieceColor);
		bool isCheck();
		bool isPinned();
		void makeMove(chess::Move);
		void unmakeMove();
	};
}