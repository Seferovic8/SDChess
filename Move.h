#pragma once
#include<iostream>
#include<string>
#include "Position.h";
#include<vector>
namespace chess {

	class Move {
	private:
		chess::Position _from, _to;
	public:
		Move(); // default constructor

		Move(chess::Position from, chess::Position to);
		Move(int fromRow, int fromCol, int toRow, int toCol);
		Move(std::string fromSquare, std::string toSquare);
		std::string getMoveText() const;
		chess::Position getFromPos() const;
		chess::Position getToPos() const;
	};
}