#include <string>
#include "Position.h"

namespace chess {

Position::Position() :row(0), column(0) {}
Position::Position(int r, int c) :row(r), column(c) {}
Position::Position(std::string squareIndex) {
	chess::Position position = indexToPos(squareIndex);
	row = position.row;
	column = position.column;
}
std::string Position::squareToString(Position position) {
	return (nameColumns[position.column] + std::to_string(8 - position.row));
}
chess::Position Position::indexToPos(std::string squareName) {
	char file = squareName[0] - 'A'; // A–H
	int rank = squareName[1] - '0'; // 1–8

	int c = (file);  // A->1, B->2, etc.
	int r = 8 - (rank);
	return chess::Position(r, c);
}
int Position::getNumberIndex() {
		int pos = row * 8 + column;
		//return pos;
		return (7 - row) * 8 + column;
}

}