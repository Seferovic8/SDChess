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
	Move::Move(chess::Position from, chess::Position to, bool enPassant):_from(from),_to(to),_enPassant(enPassant){}
	Move::Move(int fromRow, int fromCol, int toRow, int toCol, bool enPassant) :_from(chess::Position(fromRow, fromCol)), _to(chess::Position(toRow, toCol)), _enPassant(enPassant) {}
	Move::Move(std::string fromSquare, std::string toSquare, bool enPassant): _enPassant(enPassant) {
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
	bool Move::isEnPassant()const {
		return _enPassant;
	}
}