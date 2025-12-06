#pragma once
#include<iostream>
#include<string>
#include "Position.h";
#include "Piece.h";
#include<vector>
namespace chess {

	class Move {
	private:
		chess::Position _from, _to;
		bool _enPassant;
		bool _isCastling;

		chess::PieceType _promotionPiece;
	public:
		Move(); // default constructor

		Move(chess::Position from, chess::Position to, bool enPassant=false, bool isCastling = false, chess::PieceType promotionPiece = chess::PieceType::None);
		Move(int fromRow, int fromCol, int toRow, int toCol, bool enPassant = false, bool isCastling = false, chess::PieceType promotionPiece = chess::PieceType::None);
		Move(std::string fromSquare, std::string toSquare, bool enPassant = false, bool isCastling = false, chess::PieceType promotionPiece = chess::PieceType::None);
		std::string getMoveText() const;
		chess::Position getFromPos() const;
		chess::Position getToPos() const;
		chess::PieceType getPromotionPiece() const;
		bool isEnPassant() const;
		bool isCastling() const;
	};
}