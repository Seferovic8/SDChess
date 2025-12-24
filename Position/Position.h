#pragma once
#include <utility> 
#include <string> 
#include <vector> 

namespace chess {

	const std::string nameColumns[8] = { "A", "B", "C", "D","E","F","G","H" };

	class Position {
	public:
		int row;
		int column;
		Position(); //default 
		Position(int r, int c);
		Position(std::string squareIndex);
		Position(int position);
		bool operator==(const Position& other)
		{
			return ((row==other.row)&&(column==other.column));
		}
		static std::string squareToString(chess::Position position);
		static chess::Position indexToPos(std::string squareName);
		static chess::Position numToPosition(int sq);
		static int positionToNum(chess::Position pos);
		static int rowColToSquare(int row, int column);
		static int flipPosition(int sq);
		int getNumberIndex();
	};
}