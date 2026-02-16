#pragma once
#include<iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Move.h"
#include "../Square/Square.h"
#include "../Board/Board.h" // <--- Crucial! Now we know what 'Board' looks like
//
namespace chess {
	//Move::Move()
	//	: _from(chess::Position(0, 0)), _to(chess::Position(0, 0)), _enPassant(false), _isCastling(false),
	//	_promotionPiece(PieceType::None)
	//{}
	Move::Move() : data(0) {}
	Move::Move(int from, int to, bool enPassant, Castling castling, chess::PieceType promotionPiece) {
		uint16_t flags = 0;

		if (castling == Castling::King) {
			flags = KING_CASTLE;
		}
		else if (castling == Castling::Queen) { // <--- CHANGED THIS LINE
			flags = QUEEN_CASTLE;
		}

		if (enPassant) flags = EP_CAPTURE;
		if (promotionPiece == PieceType::Queen) flags = PROMOTE_QUEEN;
		if (promotionPiece == PieceType::Bishop) flags = PROMOTE_BISHOP;
		if (promotionPiece == PieceType::Rook) flags = PROMOTE_ROOK;
		if (promotionPiece == PieceType::Knight) flags = PROMOTE_KNIGHT;

		data = (from & 0x3F) | ((to & 0x3F) << 6) | flags;
	}
	void MoveList::sortCaptures(Board& board) {
		// Now valid because we included Board.h
		std::sort(moves, moves + captureEnd, [&](const Move& a, const Move& b) {
			return scoreCapture(a, board) > scoreCapture(b, board);
			});
	}

	int MoveList::scoreCapture(const Move& mv, Board& board) {
		// FIX: Use '.' operator for references, not '->'
		chess::PieceType victim = board.getSquare(mv.getToPos()).getPiece().getPieceType();
		chess::PieceType attacker = board.getSquare(mv.getFromPos()).getPiece().getPieceType();

		return (10 * getPieceValue(victim)) - getPieceValue(attacker);
	}

	Move::Move(chess::Position from, chess::Position to, bool enPassant, Castling castling, chess::PieceType promotionPiece)
		: Move(from.getNumberIndex(), to.getNumberIndex(), enPassant, castling, promotionPiece)
	{
		// Body is empty! The delegation happens after the ':'
	}

	// 3. Row/Col Constructor -> Delegates to Main
	Move::Move(int fromRow, int fromCol, int toRow, int toCol, bool enPassant, Castling castling, chess::PieceType promotionPiece)
		: Move(chess::Position::rowColToSquare(fromRow, fromCol),
			chess::Position::rowColToSquare(toRow, toCol),
			enPassant, castling, promotionPiece)
	{
	}

	// 4. String Constructor -> Delegates to Main
	Move::Move(std::string fromSquare, std::string toSquare, bool enPassant, Castling castling, chess::PieceType promotionPiece)
		: Move(chess::Position::positionToNum(chess::Position::indexToPos(fromSquare)),
			chess::Position::positionToNum(chess::Position::indexToPos(toSquare)),
			enPassant, castling, promotionPiece)
	{
	}
	std::string Move::getMoveText() {
		std::string promotion = "";
		auto prom = this->getPromotionPiece();
		if (prom != chess::PieceType::None) {
			promotion += chess::pieceTypeToChar(prom);
		}
		return Position::squareToString(this->getFromPos()) + " -> " + Position::squareToString(this->getToPos())+promotion;
	}
}