#pragma once
#include<iostream>
#include <cstring> 
#include<string>
#include "../Position/Position.h"
#include "../Piece/Piece.h"
#include<vector>
namespace chess {
	class Board;
	enum Castling {
		NONE,
		King,
		Queen
	};
	// Define flags for special moves (Bits 12-15)
	const uint16_t FLAG_MASK = 0xF000;
	const uint16_t PROMOTION_MASK = 8 << 12;
	enum MoveFlag : uint16_t {
		QUIET = 0,
		DOUBLE_PAWN_PUSH = 1 << 12,
		KING_CASTLE = 2 << 12,
		QUEEN_CASTLE = 3 << 12,
		CAPTURE = 4 << 12,
		EP_CAPTURE = 5 << 12,
		PROMOTE_KNIGHT = 8 << 12,
		PROMOTE_BISHOP = 9 << 12,
		PROMOTE_ROOK = 10 << 12,
		PROMOTE_QUEEN = 11 << 12,
		PROMOTE_N_CAP = 12 << 12,
		PROMOTE_B_CAP = 13 << 12,
		PROMOTE_R_CAP = 14 << 12,
		PROMOTE_Q_CAP = 15 << 12
	};

	class Move {
	private:
		uint16_t data;

	public:
		Move();
		bool operator==(const Move& other) const noexcept {
			return data == other.data;
		}

		Move(int fromSq, int toSq, uint16_t flags = 0) {
			data = (fromSq & 0x3F) | ((toSq & 0x3F) << 6) | flags;
		}

		Move(int from, int to, bool enPassant, Castling castling = Castling::NONE, chess::PieceType promotionPiece = chess::PieceType::None);

		Move(chess::Position from, chess::Position to, bool enPassant, Castling castling = Castling::NONE, chess::PieceType promotionPiece = chess::PieceType::None);
		Move(int fromRow, int fromCol, int toRow, int toCol, bool enPassant, Castling castling = Castling::NONE, chess::PieceType promotionPiece = chess::PieceType::None);
		Move(std::string fromSquare, std::string toSquare, bool enPassant, Castling castling = Castling::NONE, chess::PieceType promotionPiece = chess::PieceType::None);
		std::string getMoveText();

		inline int getFromSq() const { return data & 0x3F; }
		inline int getToSq() const { return (data >> 6) & 0x3F; }
		inline int getFlags() const { return data & 0xF000; }

		inline chess::Position getFromPos() const { return chess::Position(getFromSq()); }
		inline chess::Position getToPos() const { return chess::Position(getToSq()); }
		bool isCapture() const { return (data & 0xF000) & CAPTURE; }
		inline bool isCastling() const {
			uint16_t flag = data & FLAG_MASK;
			return flag == KING_CASTLE || flag == QUEEN_CASTLE;
		}
		inline bool isEnPassant() const {
			return (data & FLAG_MASK) == EP_CAPTURE;
		}
		inline chess::PieceType getPromotionPiece() const {
			uint16_t flag = data & FLAG_MASK;

			if (flag & PROMOTION_MASK) {

				switch (flag & 0xB000) {
				case PROMOTE_KNIGHT: return chess::PieceType::Knight;
				case PROMOTE_BISHOP: return chess::PieceType::Bishop;
				case PROMOTE_ROOK:   return chess::PieceType::Rook;
				case PROMOTE_QUEEN:  return chess::PieceType::Queen;
				}
			}
			return chess::PieceType::None;
		}
	};

	struct MoveList {
		Move moves[264];
		int count = 0;

		int captureEnd = 0;
		int quietStart = 264;

		void push_back(const Move& m, bool isCapture = false) {
			if (isCapture) {
				moves[captureEnd++] = m;
			}
			else {
				moves[--quietStart] = m;
			}
		}
		bool has(const Move& move) const {
			for (int i = 0; i < count; ++i) {
				auto k = moves[i];
				auto b = moves[i].getFromPos();
				//auto b = moves[i].getFromSq();
				if (moves[i] == move) {
					return true;
				}
			}
			return false;
		}

		void finalize() {
			int numQuiets = 264 - quietStart;

			if (numQuiets > 0) {
				std::memmove(&moves[captureEnd], &moves[quietStart], numQuiets * sizeof(Move));
			}

			count = captureEnd + numQuiets;
		}
		void finalizeCapturesOnly() {

		//	std::memmove(&moves[captureEnd], &moves[264], 264 * sizeof(Move));


			count = captureEnd;
		}
		Move* begin() { return &moves[0]; }
		Move* end() { return &moves[count]; }

		const Move* begin() const { return &moves[0]; }
		const Move* end() const { return &moves[count]; }
		void sortCaptures(Board& board);
		int scoreCapture(const Move& mv, Board& board);

		// Helper stays here because it doesn't need Board
		int getPieceValue(chess::PieceType pieceType) {
			switch (pieceType) {
			case chess::PieceType::Pawn: return 100;
			case chess::PieceType::Knight: return 300;
			case chess::PieceType::Bishop: return 320;
			case chess::PieceType::Rook: return 500;
			case chess::PieceType::Queen: return 900;
			case chess::PieceType::King: return 20000;
			default: return 0;
			}
		}
		template <typename Iterator>
		void insert(Move* position, Iterator first, Iterator last) {
			int numNewMoves = 0;
			Iterator temp = first;
			while (temp != last) { numNewMoves++; temp++; }

			if (count + numNewMoves > 264) return;

			int elementsToShift = end() - position;

			if (elementsToShift > 0) {
				std::memmove(position + numNewMoves, position, elementsToShift * sizeof(Move));
			}

			while (first != last) {
				*position = *first;
				position++;
				first++;
			}
			count += numNewMoves;
		}
	};

}