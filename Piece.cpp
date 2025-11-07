#include<iostream>
#include <string>
#include <vector>
#include "Square.h"


namespace chess {
	Piece::Piece(char label,Color color,PieceType pieceType):_color(color),_pieceType(pieceType) {
		if (color == Color::White) {
			_label = label;
	}
		else {
			_label = std::tolower(label);
		}
	}
	PieceType Piece::getPieceType() const {
		return _pieceType;
	}
	char Piece::getLabel() {
		return _label;
	}
	Color Piece::getColor() {
		return _color;
	}
	Pawn::Pawn(Color color):Piece('P',color,PieceType::Pawn){}
	Queen::Queen(Color color):Piece('Q', color, PieceType::Queen){}
	King::King(Color color):Piece('K', color, PieceType::King){}
	Bishop::Bishop(Color color):Piece('B', color, PieceType::Bishop){}
	Knight::Knight(Color color):Piece('N', color, PieceType::Knight){}
	Rook::Rook(Color color):Piece('R', color, PieceType::Rook){}
	std::unique_ptr<Piece> createPieceFromChar(char c) {
		Color color = std::isupper(c) ? Color::White : Color::Black;

		switch (std::tolower(c)) {
		case 'p':
			return std::make_unique<Pawn>(color);
		case 'r':
			return std::make_unique<Rook>(color);
		case 'n':
			return std::make_unique<Knight>(color);
		case 'b':
			return std::make_unique<Bishop>(color);
		case 'q':
			return std::make_unique<Queen>(color);
		case 'k':
			return std::make_unique<King>(color);
		default:
			return nullptr; // unrecognized char, no piece
		}
	}

	std::vector<chess::Position> King::directions() const {
		return {
			{1,0}, {-1,0}, {0,1}, {0,-1},     // vertical & horizontal
			{1,1}, {1,-1}, {-1,1}, {-1,-1}    // diagonals
		};
	}	

	std::vector<chess::Position> Queen::directions() const {
		return {
			{1,0}, {-1,0}, {0,1}, {0,-1},      // Rook directions (vertical + horizontal)
			{1,1}, {1,-1}, {-1,1}, {-1,-1}     // Bishop directions (diagonals)
		};

	}	
	std::vector<chess::Position> Rook::directions() const {
		return {
	{1,0}, {-1,0}, {0,1}, {0,-1}
		};
	}	
	std::vector<chess::Position> Bishop::directions() const {
		return {
	{1,1}, {1,-1}, {-1,1}, {-1,-1}
		};

	}	
	std::vector<chess::Position> Knight::directions() const {
		return {
			{2,1}, {2,-1}, {-2,1}, {-2,-1},
			{1,2}, {1,-2}, {-1,2}, {-1,-2}
		};
	}	
	std::vector<chess::Position> Pawn::directions() const {
		return {

		};


	}


}