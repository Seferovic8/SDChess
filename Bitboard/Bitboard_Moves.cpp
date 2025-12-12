#include "Bitboard.h"

namespace chess {

	uint64_t Bitboard::rookAttacksWithBlockers(int sq, uint64_t occ) {
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
	uint64_t Bitboard::bishopAttacksWithBlockers(int sq, uint64_t occ) {
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
	IndexSet Bitboard::getAllQueenMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return getBitList((rookAttacksWithBlockers(sq, allPieces) | bishopAttacksWithBlockers(sq, allPieces)) & ~myPieces);
	}
	IndexSet Bitboard::getAllBishopMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;

		return getBitList(bishopAttacksWithBlockers(sq, allPieces) & ~myPieces);
	}
	IndexSet Bitboard::getAllRookMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return getBitList(rookAttacksWithBlockers(sq, allPieces) & ~myPieces);
	}
	IndexSet Bitboard::getAllKnightMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return getBitList(knightMoves[sq] & ~myPieces);
	}
	IndexSet Bitboard::getAllKingMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return getBitList(kingMoves[sq] & ~myPieces);
	}

	uint64_t Bitboard::rayBetween(int kingSq, int attackerSq) {
		int dir = direction[kingSq][attackerSq]; // precomputed direction table
		if (dir == 0) return 0ULL; // not in same line/diagonal

		uint64_t ray = 0ULL;
		int sq = kingSq + dir;
		while (sq != attackerSq) {
			ray |= 1ULL << sq;
			sq += dir;
		}
		return ray;
	}

	MoveList Bitboard::generateKingMovesOnly(Color side) {
		MoveList moves;
		uint64_t kingBB = (side == Color::White) ? whiteKings : blackKings;
		int from = lsb(kingBB);

		uint64_t friends = (side == Color::White) ? whitePieces : blackPieces;
		uint64_t enemyAttacks = controledSquares(side == Color::White ? Color::Black : Color::White);

		// get possible king moves
		uint64_t mask = kingMoves[from];

		// remove own pieces
		mask &= ~friends;

		// remove squares attacked by opponent
		mask &= ~enemyAttacks;

		while (mask) {
			int to = lsb(mask);
			mask &= mask - 1;

			moves.push_back(Move(chess::Position::numToPosition(from), chess::Position::numToPosition(to)));   // your Move struct
		}
		return moves;
	}
}