#pragma once
#include<iostream>
#include <string>
#include <vector>
#include <list>
#include "../Position/Position.h"
#include "Square.h"



namespace chess {
	Square::Square()
		: _row(0), _column(0), _piece(Piece(chess::Color::White,chess::PieceType::None))
	{}

	Square::Square(int r, int c, Piece piece):_row(r),_column(c), _piece(piece) {
		//_index = nameColumns[c] + std::to_string(r);
	}
	Square::Square(std::string squareIndex, Piece piece) :_piece(piece) {
		chess::Position position = chess::Position::indexToPos(squareIndex);
		_row = position.row;
		_column = position.column;
	}


	char Square::getLabel()const {
		if (_piece.getPieceType() == chess::PieceType::None) {
			return '.';
		}
		
		return _piece.getLabel();
	}
	bool Square::hasPiece( )const {
		if (_piece.getPieceType() == chess::PieceType::None) {
			return false;
		}
		return true;
	}
	chess::Position Square::getPosition() {
		return chess::Position( _row, _column);
	}
	chess::Piece Square::getPiece()  {
		return _piece;
	}
	bool Square::isKing() const {
		return hasPiece() && _piece.getPieceType() == PieceType::King;
	}
	void Square::removePiece() {
		_piece = Piece(Color::White,chess::PieceType::None);
	}
	void Square::addPiece(Piece piece) {
		_piece = piece;
	}
}
//bool Board::canMove(int fr, int fc, int tr, int tc) const {
//    // get piece
//    Piece* p = board[fr][fc].getPiece();
//    if (!p) return false; // no piece there
//
//    int dr = tr - fr;
//    int dc = tc - fc;
//
//    // Get allowed move directions
//    auto dirs = p->directions();
//
//    for (auto [vr, vc] : dirs) {
//
//        // sliding piece (rook, bishop, queen)
//        if (p->isSliding()) {
//            int r = fr + vr;
//            int c = fc + vc;
//
//            while (isInside(r, c)) {
//                if (r == tr && c == tc) {
//                    // reached destination: check path
//                    if (!isPathClear(fr, fc, tr, tc))
//                        return false;
//
//                    Piece* dest = board[tr][tc].getPiece();
//                    return !dest || dest->color != p->color;
//                }
//
//                if (board[r][c].hasPiece())
//                    break;
//
//                r += vr;
//                c += vc;
//            }
//        }
//        else {
//            // non sliding piece (knight/king)
//            if (fr + vr == tr && fc + vc == tc) {
//                Piece* dest = board[tr][tc].getPiece();
//                return !dest || dest->color != p->color;
//            }
//        }
//    }
//
//    return false;
//}
