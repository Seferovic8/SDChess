#pragma once
#include<iostream>
#include<string>
#include "../Square/Square.h"
#include "../Bitboard/Bitboard.h"
#include <array>   
#include "evaluation_parameters.cpp"
#include "../Move/Move.h"
#include <vector>
namespace chess {
	using VectorBoard = Square[8][8];
	struct CastlingRights {
		bool whiteKing = true; // K
		bool whiteQueen = true; // Q
		bool blackKing = true; // k
		bool blackQueen = true; // q

		void castlingFinished(chess::Color color) {
			if(color==Color::White) {
				whiteKing = false;
				whiteQueen = false;
			}
			else {
				blackKing = false;
				blackQueen = false;
			}
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
		uint64_t previousBoardState;
		GameState() = default;
		GameState(Move m, chess::PieceType piece, Color color, CastlingRights cr, uint64_t previousBoardState, bool cap = false, bool en = false, bool prom = false, bool cast = false, chess::PieceType pt = chess::PieceType::None)
			: move(m), playedPiece(piece), moveColor(color), previousCastlingRights(cr), isCapture(cap), enPassant(en), isPromotion(prom), isCastling(cast), capturedPiece(pt), previousBoardState(previousBoardState)
		{}
	};
	enum Check{Undefined,Yes,No};
	class Board {
	private:
		VectorBoard board;
		Check check;
		Bitboard bitboard;
		std::vector<GameState> history;
		CastlingRights castling;
		Weights weights;
	public:
		Color sideToMove;
		//Board.cpp
		Board();
		Board(std::string fen);
		static std::string generateChess960FEN();
		std::vector< std::vector<Square>> getBoard();
		void printBoard() const;
		void buildFromBitboards();
		Square getSquare(chess::Position pos);
		bool isInside(int r, int c);
		bool isRepetition();
		void makeMove(chess::Move);
		void unmakeMove();
		std::pair<Move, int> findBestMove(int depth);
		int minimax_alpha_beta(int depth, int alpha, int beta);
		int quiescence_search(int alpha, int beta);
		//Board_MoveGen.cpp
		void generatePawnMoves(int sq, MoveList& moves);
		MoveList getAllPseudoLegalMoves();
		MoveList getAllLegalMoves();
		MoveList getAllLegalCaptures();
		void rookCastling(chess::Position pos, Color pieceColor);
		bool isCheck();
		bool isCheckMate();
		std::string moveToSAN(const chess::Move& mv);
		double estimatePositions(int depth);
		//Board_Evaluation.cpp
		int evaluate();
		int getMaterialScore(Color color);
		int getMobilityScore(Color color);
		std::array<int,3> getPestoScore(Color color);
		int getPositionalScore();

	};

}