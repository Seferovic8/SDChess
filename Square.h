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
		std::unique_ptr<Piece> _piece;
	public:
		Square(); // default constructor

		Square(int row, int column, std::unique_ptr<Piece> piece =nullptr);
		Square(std::string squareIndex, std::unique_ptr<Piece> piece =nullptr);
		char getLabel() const;
		bool hasPiece() const;
		chess::Position getPosition();
		Piece* getPiece();
		void removePiece();
		void addPiece(std::unique_ptr<Piece> piece);
		bool isKing() const;
	};
}