#pragma once
#include<iostream>
#include <string>
#include <vector>
#include "Move.h"
#include "Square.h"

//
namespace chess {
	Move::Move()
		: _from(chess::Position(0,0)) , _to(chess::Position(0, 0))
	{}
	Move::Move(chess::Position from, chess::Position to):_from(from),_to(to){}
	Move::Move(int fromRow, int fromCol, int toRow, int toCol) :_from(chess::Position(fromRow, fromCol)), _to(chess::Position(toRow, toCol)) {}
	Move::Move(std::string fromSquare, std::string toSquare) {
		_from = Position::indexToPos(fromSquare);
		_to = Position::indexToPos(toSquare);

	}
	std::string Move::getMoveText() const {
		return Position::squareToString(_from) + +" -> " + Position::squareToString(_to);
	}
	chess::Position Move::getFromPos()const {
		return _from;
	}
	chess::Position Move::getToPos()const {
		return _to;
	}
}