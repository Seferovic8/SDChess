#include "Bitboard.h"
namespace chess {
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
	uint64_t Bitboard::getMyPieceBitboard(Color color)  const {
		//pBitboard(blackPieces);
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
				uint64_t attacks = initRookAttacksWithBlockers(sq, blockers);

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
				uint64_t attacks = initBishopAttacksWithBlockers(sq, blockers);

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
	uint64_t Bitboard::initRookAttacksWithBlockers(int sq, uint64_t occ) {
		uint64_t attacks = 0ULL;

		// extract rays
		uint64_t rMask = rookRays[sq];

		// up
		for (int r = sq + 8; r < 64; r += 8) {
			attacks |= 1ULL << r;
			if (occ & (1ULL << r)) break;
		}
		// down
		for (int r = sq - 8; r >= 0; r -= 8) {
			attacks |= 1ULL << r;
			if (occ & (1ULL << r)) break;
		}
		// right
		for (int c = sq + 1; (c % 8) != 0; c++) {
			attacks |= 1ULL << c;
			if (occ & (1ULL << c)) break;
		}
		// left
		for (int c = sq - 1; (c % 8) != 7 && c >= 0; c--) {
			attacks |= 1ULL << c;
			if (occ & (1ULL << c)) break;
		}

		return attacks;
	}
	uint64_t Bitboard::initBishopAttacksWithBlockers(int sq, uint64_t occ) {
		uint64_t attacks = 0ULL;

		// NE
		for (int r = sq + 9; r < 64 && (r % 8) != 0; r += 9) {
			attacks |= 1ULL << r;
			if (occ & (1ULL << r)) break;
		}
		// NW
		for (int r = sq + 7; r < 64 && (r % 8) != 7; r += 7) {
			attacks |= 1ULL << r;
			if (occ & (1ULL << r)) break;
		}
		// SE
		for (int r = sq - 7; r >= 0 && (r % 8) != 0; r -= 7) {
			attacks |= 1ULL << r;
			if (occ & (1ULL << r)) break;
		}
		// SW
		for (int r = sq - 9; r >= 0 && (r % 8) != 7; r -= 9) {
			attacks |= 1ULL << r;
			if (occ & (1ULL << r)) break;
		}

		return attacks;
	}
}