#pragma once
#include<iostream>
#include <string>
#include <vector>
#include "Move.h"
#include "Square.h"

//
namespace chess {
	//Move::Move()
	//	: _from(chess::Position(0, 0)), _to(chess::Position(0, 0)), _enPassant(false), _isCastling(false),
	//	_promotionPiece(PieceType::None)
	//{}
	Move::Move() : _from(chess::Position(0, 0)), _to(chess::Position(0, 0)), _enPassant(false), _isCastling(false), _promotionPiece(PieceType::None) {}
	Move::Move(chess::Position from, chess::Position to, bool enPassant, bool isCastling, chess::PieceType promotionPiece):_from(from),_to(to),
	_enPassant(enPassant),_isCastling(isCastling), _promotionPiece(promotionPiece) {}

	Move::Move(int fromRow, int fromCol, int toRow, int toCol, bool enPassant, bool isCastling, chess::PieceType promotionPiece) :_from(chess::Position(fromRow, fromCol)), _to(chess::Position(toRow, toCol)),
		_enPassant(enPassant), _isCastling(isCastling), _promotionPiece(promotionPiece) {}
	Move::Move(std::string fromSquare, std::string toSquare, bool enPassant, bool isCastling, chess::PieceType promotionPiece)
		: _enPassant(enPassant), _isCastling(isCastling), _promotionPiece(promotionPiece) {
		_from = Position::indexToPos(fromSquare);
		_to = Position::indexToPos(toSquare);

	}
	std::string Move::getMoveText() const {
		return Position::squareToString(_from) + " -> " + Position::squareToString(_to);
	}
	chess::Position Move::getFromPos()const {
		return _from;
	}
	chess::Position Move::getToPos()const {
		return _to;
	}
	bool Move::isCastling() const {
		return _isCastling;
	}
	chess::PieceType Move::getPromotionPiece() const {
		return _promotionPiece;
	}
	bool Move::isEnPassant() const {
		return _enPassant;
	}
}