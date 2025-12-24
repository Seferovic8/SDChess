#pragma once
#include<iostream>
#include<string>
#include "../Position/Position.h";
#include "../Piece/Piece.h";
#include<vector>
namespace chess {

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
            // THE MAGIC: Only 2 bytes!
            uint16_t data;

        public:
            Move();

            // Fast Constructor (Use this in your move generator!)
            Move(int fromSq, int toSq, uint16_t flags = 0) {
                data = (fromSq & 0x3F) | ((toSq & 0x3F) << 6) | flags;
            }

            // Compatibility Constructor (Slower, keeps your old code working)
            Move(int from, int to, bool enPassant, Castling castling = Castling::NONE, chess::PieceType promotionPiece = chess::PieceType::None);

            Move(chess::Position from, chess::Position to, bool enPassant, Castling castling=Castling::NONE, chess::PieceType promotionPiece = chess::PieceType::None);
            Move(int fromRow, int fromCol, int toRow, int toCol, bool enPassant, Castling castling = Castling::NONE, chess::PieceType promotionPiece = chess::PieceType::None);
            Move(std::string fromSquare, std::string toSquare, bool enPassant, Castling castling = Castling::NONE, chess::PieceType promotionPiece = chess::PieceType::None);
            std::string getMoveText();

            // --- Getters (Compute on the fly) ---

            // Returns raw integer index (0-63). FAST.
            inline int getFromSq() const { return data & 0x3F; }
            inline int getToSq() const { return (data >> 6) & 0x3F; }
            inline int getFlags() const { return data & 0xF000; }

            // Compatibility Getter (Slow, creates object)
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

                // Optimization: All promotion flags have the 4th bit set (8, 9, 10, 11, etc.)
                if (flag & PROMOTION_MASK) {
                    // Mask out the "Capture" bit (bit 14) so we treat 
                    // "Promote Queen" and "Promote Queen w/ Capture" the same.
                    switch (flag & 0xB000) { // 0xB000 masks out the capture bit (0100)
                    case PROMOTE_KNIGHT: return chess::PieceType::Knight;
                    case PROMOTE_BISHOP: return chess::PieceType::Bishop;
                    case PROMOTE_ROOK:   return chess::PieceType::Rook;
                    case PROMOTE_QUEEN:  return chess::PieceType::Queen;
                    }
                }
                return chess::PieceType::None;
            }
            // ... etc
        };
  
	struct MoveList {
		Move moves[264];
		int count = 0;
		void push_back(const Move& m) {
			moves[count++] = m;
		}
		Move* begin() {
			return &moves[0];
		}

		Move* end() {
			return &moves[count];
		}

		const Move* begin() const {
			return &moves[0];
		}
		const Move* end() const {
			return &moves[count];
		}
		template <typename Iterator>
		void insert(Move* position, Iterator first, Iterator last) {


			while (first != last) {
				// Safety check: stop if we hit the array limit
				if (count >= 264) break;

				moves[count] = *first;
				count++;
				++first;
			}
		}
	};
}