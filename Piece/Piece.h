#pragma once
#include<iostream>
#include<string>
#include<vector>
#include "../Position/Position.h";
namespace chess {
	const std::vector<chess::Position> KNIGHT_DIRS = { {2,1}, {2,-1}, {-2,1}, {-2,-1}, {1,2}, {1,-2}, {-1,2}, {-1,-2} };
	const std::vector<chess::Position> BISHOP_DIRS = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };
	const std::vector<chess::Position> ROOK_DIRS = { {1,0}, {-1,0}, {0,1}, {0,-1} };
	const std::vector<chess::Position> QUEEN_DIRS = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };
	const std::vector<chess::Position> KING_DIRS = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1} };
	const std::vector<chess::Position> NO_DIRS = {};
	enum class Color { White=0, Black=1 };
	inline Color operator!(Color c) {
		return (c == Color::White) ? Color::Black : Color::White;
	}
	enum class PieceType { Pawn=0, Knight=1, Bishop=2, Rook=3, Queen=4, King=5, None=6 };
	class Piece;
	Piece createPieceFromChar(char c);
	char getPieceChar(PieceType pieceType);
	Piece createPiece(chess::PieceType pieceType, chess::Color color);

	class Piece {
	protected:
		char _label;
		Color _color;
		PieceType _pieceType;
	public:
		~Piece() = default;
		bool isSliding() const;
		PieceType getPieceType() const;

		std::vector<chess::Position> directions() const;
		Color getColor();
		char getLabel() const;
		Piece(Color color,PieceType pieceType);
	};

}

