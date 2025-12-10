#pragma once
#include<iostream>
#include<string>
#include "Position.h";
#include "Piece.h";
#include<vector>
namespace chess {

	class Move {
	private:
		chess::Position _from, _to;
		bool _enPassant;
		bool _isCastling;

		chess::PieceType _promotionPiece;
	public:
		Move(); // default constructor

		Move(chess::Position from, chess::Position to, bool enPassant=false, bool isCastling = false, chess::PieceType promotionPiece = chess::PieceType::None);
		Move(int fromRow, int fromCol, int toRow, int toCol, bool enPassant = false, bool isCastling = false, chess::PieceType promotionPiece = chess::PieceType::None);
		Move(std::string fromSquare, std::string toSquare, bool enPassant = false, bool isCastling = false, chess::PieceType promotionPiece = chess::PieceType::None);
		std::string getMoveText() const;
		chess::Position getFromPos() const;
		chess::Position getToPos() const;
		chess::PieceType getPromotionPiece() const;
		bool isEnPassant() const;
		bool isCastling() const;
	};
	struct MoveList {
		Move moves[264];
		int count = 0;
		void push_back(const Move& m) {
			moves[count++] = m;
		}
		Move* begin() {
			return &moves[0];
		}

		Move* end() {
			return &moves[count];
		}

		const Move* begin() const {
			return &moves[0];
		}
		const Move* end() const {
			return &moves[count];
		}
		template <typename Iterator>
		void insert(Move* position, Iterator first, Iterator last) {


			while (first != last) {
				// Safety check: stop if we hit the array limit
				if (count >= 264) break;

				moves[count] = *first;
				count++;
				++first;
			}
		}
	};
}