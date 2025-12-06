#pragma once
#include<iostream>
#include<string>
#include<vector>
#include "Position.h";
namespace chess {
	enum class Color { White, Black };
	inline Color operator!(Color c) {
		return (c == Color::White) ? Color::Black : Color::White;
	}
	enum class PieceType { Pawn, Knight, Bishop, Rook, Queen, King, None };
	class Piece;
	std::unique_ptr<Piece> createPieceFromChar(char c);
	std::unique_ptr<Piece> createPiece(chess::PieceType pieceType, chess::Color color);

	class Piece {
	protected:
		char _label;
		Color _color;
		PieceType _pieceType;
	public:
		virtual ~Piece() = default;
		virtual bool isSliding() const = 0;
		virtual PieceType getPieceType() const;

		virtual std::vector<chess::Position> directions() const = 0;
		Color getColor();
		char getLabel();
		Piece(char label,Color color,PieceType pieceType);
	};
	class Pawn : public Piece {
	public:
		int moveCount = 0;
		Pawn(Color color);
		bool isSliding() const override { return false; } 
		std::vector<chess::Position> directions() const override;
	};
	class King : public Piece {
	public:
		King(Color color);
		bool isSliding() const override { return false; } 
		std::vector<chess::Position> directions() const override;
	};
	class Queen : public Piece {
	public:
		Queen(Color color);
		bool isSliding() const override { return true; } 
		std::vector<chess::Position> directions() const override;
	};
	class Bishop : public Piece {
	public:
		Bishop(Color color);
		bool isSliding() const override { return true; } 
		std::vector<chess::Position> directions() const override;
	};
	class Knight : public Piece {
	public:
		Knight(Color color);
		bool isSliding() const override { return false; } 
		std::vector<chess::Position> directions() const override;
	};
	class Rook : public Piece{
	public:
		Rook(Color color);
		bool isSliding() const override { return true; } 
		std::vector<chess::Position> directions() const override;
	};
}

