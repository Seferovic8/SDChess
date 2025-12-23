#include "Bitboard.h"

namespace chess {


	uint64_t Bitboard::getRookAttacks(int sq,uint64_t occ, uint64_t myPieces) {

			 occ = occ & rookMask[sq];
			uint64_t index = (occ * rookMagic[sq]) >> rookShift[sq];
			uint64_t attacks = rookAttacks[sq][index];
			attacks &= ~myPieces;
			return attacks;
		
	}
	uint64_t Bitboard::getBishopAttacks(int sq, uint64_t occ, uint64_t myPieces) {

		 occ = occ & bishopMask[sq];
		uint64_t index = (occ * bishopMagic[sq]) >> bishopShift[sq];
		uint64_t attacks = bishopAttacks[sq][index];
		attacks &= ~myPieces;
		return attacks;
	}

	uint64_t Bitboard::getAllQueenMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return (getRookAttacks(sq,allPieces, myPieces) | getBishopAttacks(sq, allPieces, myPieces));
		//return getBitList((getRookAttacks(sq,allPieces, myPieces) | getBishopAttacks(sq, allPieces, myPieces)));
	}
	uint64_t Bitboard::getAllBishopMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;

		return getBishopAttacks(sq, allPieces, myPieces);
		//return getBitList(getBishopAttacks(sq, allPieces, myPieces));
	}
	uint64_t Bitboard::getAllRookMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;

		return getRookAttacks(sq, allPieces, myPieces);
		//return getBitList(getRookAttacks(sq, allPieces, myPieces));
	}
	uint64_t Bitboard::getAllKnightMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return knightMoves[sq] & ~myPieces;
		//return getBitList(knightMoves[sq] & ~myPieces);
	}
	uint64_t Bitboard::getAllKingMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return kingMoves[sq] & ~myPieces;
		//return getBitList(kingMoves[sq] & ~myPieces);
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

	MoveList Bitboard::generateKingMovesOnly(int sq, chess::Color color) {
		MoveList moves;

		uint64_t friends = (color == Color::White) ? whitePieces : blackPieces;
		uint64_t enemyAttacks = controledSquares(color == Color::White ? Color::Black : Color::White);

		// get possible king moves
		uint64_t mask = kingMoves[sq];

		// remove own pieces
		mask &= ~friends;

		// remove squares attacked by opponent
		mask &= ~enemyAttacks;
		while (mask) {
			int to = lsb(mask);
			mask &= mask - 1;

			moves.push_back(Move(chess::Position::numToPosition(sq), chess::Position::numToPosition(to)));   // your Move struct
		}
		return moves;
	}
}