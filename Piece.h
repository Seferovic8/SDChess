#pragma once
#include<iostream>
#include<string>
#include<vector>
#include "Position.h";
namespace chess {
	enum class Color { White, Black };
	inline Color operator!(Color c) {
		return (c == Color::White) ? Color::Black : Color::White;
	}
	enum class PieceType { Pawn, Knight, Bishop, Rook, Queen, King, None };
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

