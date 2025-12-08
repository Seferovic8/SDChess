#pragma once
#include<iostream>
#include<string>
#include "Position.h";
#include "Piece.h"
#include<vector>
namespace chess {

	class Square {
	private: 
		int _row, _column;
		std::string _index;
		Piece _piece;
	public:
		Square(); // default constructor

		Square(int row, int column, Piece piece =Piece(chess::Color::White,chess::PieceType::None));
		Square(std::string squareIndex, Piece piece = Piece(chess::Color::White, chess::PieceType::None));
		char getLabel() const;
		bool hasPiece() const;
		chess::Position getPosition();
		Piece getPiece();
		void removePiece();
		void addPiece(Piece piece);
		bool isKing() const;
	};
}