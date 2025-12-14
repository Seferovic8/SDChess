#include<iostream>
#include <string>
#include <vector>
#include <array>

#include "Bitboard.h"
#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace chess {
	uint64_t Bitboard::knightMoves[64];
	uint64_t Bitboard::bishopRays[64];
	uint64_t Bitboard::rookRays[64];
	uint64_t Bitboard::pawnAttacksWhite[64];
	uint64_t Bitboard::pawnAttacksBlack[64];
	uint64_t Bitboard::kingMoves[64];
	uint64_t Bitboard::rookMask[64];
	uint64_t Bitboard::bishopMask[64];
	uint64_t Bitboard::rookAttacks[64][4096];
	uint64_t Bitboard::bishopAttacks[64][4096];
	int Bitboard::direction[64][64];
	Bitboard::Bitboard() {
		initAttackTables();
	}
	Bitboard::Bitboard(chess::VectorBoard& board, chess::Color side) {
		initAttackTables();
		loadBitboard(board,side);
	}
	void Bitboard::loadBitboard(chess::VectorBoard& board,Color side) {
		sideToMove = side;

		whitePawns = whiteKnights = whiteBishops = whiteRooks = whiteQueens = whiteKings = 0ULL;
		blackPawns = blackKnights = blackBishops = blackRooks = blackQueens = blackKings = 0ULL;
		for (int r = 0; r < 8; r++) {
			for (int c = 0; c < 8; c++) {
				if (!board[r][c].hasPiece())
					continue;

				auto piece = board[r][c].getPiece();
				int sq = (7 - r) * 8 + c; // convert to 0–63

				if (piece.getColor() == Color::White) {
					if (piece.getPieceType() == PieceType::Pawn)  whitePawns |= (1ULL << sq);
					else if (piece.getPieceType() == PieceType::Knight) whiteKnights |= (1ULL << sq);
					else if (piece.getPieceType() == PieceType::Bishop) whiteBishops |= (1ULL << sq);
					else if (piece.getPieceType() == PieceType::Rook)   whiteRooks |= (1ULL << sq);
					else if (piece.getPieceType() == PieceType::Queen)  whiteQueens |= (1ULL << sq);
					else if (piece.getPieceType() == PieceType::King)   whiteKings |= (1ULL << sq);
				}
				else { // Black pieces
					if (piece.getPieceType() == PieceType::Pawn)   blackPawns |= (1ULL << sq);
					else if (piece.getPieceType() == PieceType::Knight) blackKnights |= (1ULL << sq);
					else if (piece.getPieceType() == PieceType::Bishop) blackBishops |= (1ULL << sq);
					else if (piece.getPieceType() == PieceType::Rook)   blackRooks |= (1ULL << sq);
					else if (piece.getPieceType() == PieceType::Queen)  blackQueens |= (1ULL << sq);
					else if (piece.getPieceType() == PieceType::King)   blackKings |= (1ULL << sq);
				}
			}
		}

		// Combined maps
		whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKings;
		blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKings;
		allPieces = whitePieces | blackPieces;

		material[0][0] = getBitList(whitePawns);
		material[0][1] = getBitList(whiteKnights);
		material[0][2] = getBitList(whiteBishops);
		material[0][3] = getBitList(whiteRooks);
		material[0][4] = getBitList(whiteQueens);
		material[0][5] = getBitList(whiteKings);

		material[1][0] = getBitList(blackPawns);
		material[1][1] = getBitList(blackKnights);
		material[1][2] = getBitList(blackBishops);
		material[1][3] = getBitList(blackRooks);
		material[1][4] = getBitList(blackQueens);
		material[1][5] = getBitList(blackKings);
	}


	void Bitboard::pBitboard(uint64_t allPieces)
	{
		std::cout << std::endl;

		for (int rank = 7; rank >= 0; rank--)     // 8 → 1
		{
			for (int file = 0; file < 8; file++)  // a → h
			{
				int sq = rank * 8 + file;
				uint64_t mask = 1ULL << sq;

				std::cout << ((allPieces & mask) ? "1 " : ". ");
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}
	void Bitboard::printBitboard() const
	{

		for (int rank = 7; rank >= 0; rank--)     // 8 → 1
		{
			for (int file = 0; file < 8; file++)  // a → h
			{
				int sq = rank * 8 + file;
				uint64_t mask = 1ULL << sq;

				std::cout << ((allPieces & mask) ? "1 " : ". ");
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}
	void Bitboard::initAttackTables() {
		initKnightMoves();
		initKingMoves();
		initPawnAttacks();
		initRookRays();
		initBishopRays();
		initDirectionTable();
	}
	void Bitboard::initDirectionTable() {
		for (int from = 0; from < 64; from++) {
			for (int to = 0; to < 64; to++) {

				direction[from][to] = 0; // default

				if (from == to) continue;

				int fromFile = from % 8, fromRank = from / 8;
				int toFile = to % 8, toRank = to / 8;

				int df = toFile - fromFile;
				int dr = toRank - fromRank;

				// same file (vertical)
				if (df == 0)
					direction[from][to] = (dr > 0) ? +8 : -8;

				// same rank (horizontal)
				else if (dr == 0)
					direction[from][to] = (df > 0) ? +1 : -1;

				// diagonal
				else if (abs(df) == abs(dr)) {
					if (df > 0 && dr > 0) direction[from][to] = +9; // NE
					if (df < 0 && dr > 0) direction[from][to] = +7; // NW
					if (df > 0 && dr < 0) direction[from][to] = -7; // SE
					if (df < 0 && dr < 0) direction[from][to] = -9; // SW
				}
			}
		}
	}
	void Bitboard::initKnightMoves() {
		for (int sq = 0; sq < 64; sq++) {
			int r = sq / 8, c = sq % 8;
			uint64_t mask = 0ULL;

			auto add = [&](int rr, int cc) {
				if (rr >= 0 && rr < 8 && cc >= 0 && cc < 8)
					mask |= 1ULL << (rr * 8 + cc);
			};

			add(r + 2, c + 1); add(r + 2, c - 1);
			add(r - 2, c + 1); add(r - 2, c - 1);
			add(r + 1, c + 2); add(r + 1, c - 2);
			add(r - 1, c + 2); add(r - 1, c - 2);

			knightMoves[sq] = mask;
		}
	}
	void Bitboard::initPawnAttacks() {
		for (int sq = 0; sq < 64; sq++) {
			uint64_t bb = 1ULL << sq;

			pawnAttacksWhite[sq] =
				((bb & ~FILE_A) << 7) | ((bb & ~FILE_H) << 9);

			pawnAttacksBlack[sq] =
				((bb & ~FILE_H) >> 7) | ((bb & ~FILE_A) >> 9);
		}
	}
	uint64_t Bitboard::getMyPieceBitboard(Color color) const {
		if (color == Color::White) {
			return whitePieces;
		}
		return blackPieces;
	}
	void Bitboard::initRookRays() {
		for (int sq = 0; sq < 64; sq++) {
			int r = sq / 8, c = sq % 8;
			uint64_t mask = 0ULL;

			for (int rr = r + 1; rr < 8; rr++) mask |= 1ULL << (rr * 8 + c);
			for (int rr = r - 1; rr >= 0; rr--) mask |= 1ULL << (rr * 8 + c);
			for (int cc = c + 1; cc < 8; cc++) mask |= 1ULL << (r * 8 + cc);
			for (int cc = c - 1; cc >= 0; cc--) mask |= 1ULL << (r * 8 + cc);

			rookRays[sq] = mask;
		}
		for (int sq = 0; sq < 64; sq++) {
			int r = sq / 8, c = sq % 8;
			uint64_t mask = 0ULL;

			for (int rr = r + 1; rr < 7; rr++) mask |= 1ULL << (rr * 8 + c);
			for (int rr = r - 1; rr >= 1; rr--) mask |= 1ULL << (rr * 8 + c);
			for (int cc = c + 1; cc < 7; cc++) mask |= 1ULL << (r * 8 + cc);
			for (int cc = c - 1; cc >= 1; cc--) mask |= 1ULL << (r * 8 + cc);

			rookMask[sq] = mask;
		}

		for (int sq = 0; sq < 64; sq++) {

			uint64_t mask = rookMask[sq];
			int relevantBits = lsb(mask);
			int tableSize = 1 << relevantBits;

			// Clear table (optional but safe)
			for (int i = 0; i < 4096; i++)
				rookAttacks[sq][i] = 0ULL;

			uint64_t blockers = 0ULL;
			do {
				uint64_t attacks = rookAttacksWithBlockers(sq, blockers);

				uint64_t index =
					(blockers * rookMagic[sq]) >> (rookShift[sq]);

				rookAttacks[sq][index] = attacks;

				blockers = (blockers - mask) & mask;
			} while (blockers);
		}
	}
	void Bitboard::initBishopRays() {
		for (int sq = 0; sq < 64; sq++) {
			int r = sq / 8, c = sq % 8;
			uint64_t mask = 0ULL;

			for (int rr = r + 1, cc = c + 1; rr < 8 && cc < 8; rr++, cc++) mask |= 1ULL << (rr * 8 + cc);
			for (int rr = r + 1, cc = c - 1; rr < 8 && cc >= 0; rr++, cc--) mask |= 1ULL << (rr * 8 + cc);
			for (int rr = r - 1, cc = c + 1; rr >= 0 && cc < 8; rr--, cc++) mask |= 1ULL << (rr * 8 + cc);
			for (int rr = r - 1, cc = c - 1; rr >= 0 && cc >= 0; rr--, cc--) mask |= 1ULL << (rr * 8 + cc);

			bishopRays[sq] = mask;
		}
		for (int sq = 0; sq < 64; sq++) {
			int r = sq / 8, c = sq % 8;
			uint64_t mask = 0ULL;

			for (int rr = r + 1, cc = c + 1; rr < 7 && cc < 7; rr++, cc++) mask |= 1ULL << (rr * 8 + cc);
			for (int rr = r + 1, cc = c - 1; rr < 7 && cc >= 1; rr++, cc--) mask |= 1ULL << (rr * 8 + cc);
			for (int rr = r - 1, cc = c + 1; rr >= 1 && cc < 7; rr--, cc++) mask |= 1ULL << (rr * 8 + cc);
			for (int rr = r - 1, cc = c - 1; rr >= 1 && cc >= 1; rr--, cc--) mask |= 1ULL << (rr * 8 + cc);

			bishopMask[sq] = mask;
		}
		for (int sq = 0; sq < 64; sq++) {

			uint64_t mask = bishopMask[sq];
			int relevantBits = lsb(mask);
			int tableSize = 1 << relevantBits;

			// Clear table (optional but safe)
			for (int i = 0; i < 4096; i++)
				bishopAttacks[sq][i] = 0ULL;

			uint64_t blockers = 0ULL;
			do {
				uint64_t attacks = bishopAttacksWithBlockers(sq, blockers);

				uint64_t index =
					(blockers * bishopMagic[sq]) >> bishopShift[sq];

				bishopAttacks[sq][index] = attacks;

				blockers = (blockers - mask) & mask;
			} while (blockers);
		}
	}
	void Bitboard::initKingMoves() {
		for (int sq = 0; sq < 64; sq++) {
			int r = sq / 8, c = sq % 8;
			uint64_t mask = 0ULL;

			for (int dr = -1; dr <= 1; dr++)
				for (int dc = -1; dc <= 1; dc++)
					if (dr || dc) {
						int rr = r + dr, cc = c + dc;
						if (rr >= 0 && rr < 8 && cc >= 0 && cc < 8)
							mask |= 1ULL << (rr * 8 + cc);
					}

			kingMoves[sq] = mask;
		}
	}
	

	int Bitboard::getKingIndex(Color color) const {
		uint64_t kings = (color == Color::White) ? whiteKings : blackKings;
		return lsb(kings);
	}
	
	int Bitboard::getIndex(uint64_t bb)const {
		return lsb(bb);
	}


	IndexSet Bitboard::getBitList(uint64_t bb) {
		IndexSet lista;
		while (bb) {
			int sq = lsb(bb);   // index of least-significant 1-bit
			bb &= bb - 1;       // remove that bit
			lista.insert(sq);
		}
		return lista;
	}
}
