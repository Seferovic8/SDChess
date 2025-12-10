#include<iostream>
#include <string>
#include <vector>
#include "Square.h"


namespace chess {
	Piece::Piece(Color color,PieceType pieceType):_color(color),_pieceType(pieceType) {
		char label = getPieceChar(pieceType);
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
	char Piece::getLabel() const {
		return _label;
	}
	Color Piece::getColor() {
		return _color;
	}
	bool Piece::isSliding() const {
		if (_pieceType == chess::PieceType::Bishop || _pieceType == chess::PieceType::Rook || _pieceType == chess::PieceType::Queen) return true;
		return false;
	}
	std::vector<chess::Position> Piece::directions() const {
		switch (_pieceType) {
		case chess::PieceType::Knight:
			return KNIGHT_DIRS; // Returns a reference (Instant, 0 allocation)

		case chess::PieceType::Bishop:
			return BISHOP_DIRS;

		case chess::PieceType::Rook:
			return ROOK_DIRS;

		case chess::PieceType::Queen:
			return QUEEN_DIRS;

		case chess::PieceType::King:
			return KING_DIRS;

		case chess::PieceType::Pawn:
		case chess::PieceType::None:
		default:
			return NO_DIRS;
		}
	};


	char getPieceChar(chess::PieceType pieceType) {
		switch (pieceType)
		{
		case chess::PieceType::Pawn:
			return 'P';
			break;
		case chess::PieceType::Knight:
			return 'N';
			break;
		case chess::PieceType::Bishop:
			return 'B';
			break;
		case chess::PieceType::Rook:
			return 'R';
			break;
		case chess::PieceType::Queen:
			return 'Q';
			break;
		case chess::PieceType::King:
			return 'K';
			break;
		case chess::PieceType::None:
			return 'none';
			break;
		default:
			break;
		}
	}
	Piece createPieceFromChar(char c) {
		Color color = std::isupper(c) ? Color::White : Color::Black;

		switch (std::tolower(c)) {
		case 'p':
			return Piece(color,chess::PieceType::Pawn);
		case 'r':
			return Piece(color, chess::PieceType::Rook);
		case 'n':
			return Piece(color, chess::PieceType::Knight);
		case 'b':
			return Piece(color, chess::PieceType::Bishop);
		case 'q':
			return Piece(color, chess::PieceType::Queen);
		case 'k':
			return Piece(color, chess::PieceType::King);
		default:
			return Piece(color, chess::PieceType::None); // unrecognized char, no piece
		}
	}
	Piece createPiece(chess::PieceType pieceType, chess::Color color) {

		switch (pieceType) {
		case chess::PieceType::Pawn:
			return Piece(color, chess::PieceType::Pawn);
		case chess::PieceType::Rook:
			return Piece(color, chess::PieceType::Rook);
		case chess::PieceType::Knight:
			return Piece(color, chess::PieceType::Knight);
		case chess::PieceType::Bishop:
			return Piece(color, chess::PieceType::Bishop);
		case chess::PieceType::Queen:
			return Piece(color, chess::PieceType::Queen);
		case chess::PieceType::King:
			return Piece(color, chess::PieceType::King);
		default:
			return Piece(color, chess::PieceType::None); // unrecognized char, no piece
		}
	}


}