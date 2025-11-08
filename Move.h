#pragma once
#include<iostream>
#include<string>
#include "Position.h"
#include<vector>
namespace chess {

	class Move {
	private:
		chess::Position _from, _to;
		bool _enPassant;
	public:
		Move(); // default constructor

		Move(chess::Position from, chess::Position to, bool enPassant=false);
		Move(int fromRow, int fromCol, int toRow, int toCol, bool enpassant = false);
		Move(std::string fromSquare, std::string toSquare, bool enPassant = false);
		std::string getMoveText() const;
		chess::Position getFromPos() const;
		chess::Position getToPos() const;
		bool isEnPassant() const;
	};
}