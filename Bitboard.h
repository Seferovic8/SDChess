#pragma once
#include<iostream>
#include <array>          // ✅ MUST be first or at least before using std::array
#include <unordered_set>

#include<string>
#include "Position.h"
#include "Square.h"
#include "Move.h"
#include<vector>
namespace chess {
	struct IndexSet {
		int numbers[64];
		int n = 0;           // ✅ Renamed from 'count' to 'n'
		uint64_t mask = 0;

		void insert(int m) {
			if ((mask >> m) & 1) return;

			mask |= (1ULL << m);
			numbers[n++] = m; // Update to use 'n'
		}

		// Now this function name is valid because the variable is named 'n'
		int count(int m) const {
			return (mask >> m) & 1;
		}

		int size() const { return n; } // Update to use 'n'

		// Iterators
		int* begin() { return &numbers[0]; }
		int* end() { return &numbers[n]; }
		const int* begin() const { return &numbers[0]; }
		const int* end()   const { return &numbers[n]; }
	};
	using VectorBoard = std::vector<std::vector<Square>>;


	class Bitboard {
	private:
		uint64_t whitePawns = 0LL;
		uint64_t whiteQueens = 0LL;
		uint64_t whiteKings = 0LL;
		uint64_t whiteBishops = 0LL;
		uint64_t whiteKnights = 0LL;
		uint64_t whiteRooks = 0LL;

		uint64_t blackPawns = 0LL;
		uint64_t blackQueens = 0LL;
		uint64_t blackKings = 0LL;
		uint64_t blackBishops = 0LL;
		uint64_t blackKnights = 0LL;
		uint64_t blackRooks = 0LL;

		uint64_t whitePieces = 0LL;
		uint64_t blackPieces = 0LL;
		
		uint64_t allPieces = 0LL;


		uint64_t knightMoves[64];
		uint64_t bishopRays[64];
		uint64_t rookRays[64];
		uint64_t pawnAttacksWhite[64];
		uint64_t pawnAttacksBlack[64];
		uint64_t kingMoves[64];
		int direction[64][64];


		chess::Color sideToMove;
		static const uint64_t FILE_A = 0x0101010101010101ULL;
		static const uint64_t FILE_H = 0x8080808080808080ULL;
		uint64_t rookAttacksWithBlockers(int sq, uint64_t occ);
		uint64_t bishopAttacksWithBlockers(int sq, uint64_t occ);
		bool isAttacked(int square, Color color, uint64_t enemyPawns, uint64_t enemyKnights, uint64_t enemyKings, uint64_t enemyRooks, uint64_t enemyBisops, uint64_t enemyQueens, uint64_t allMyPieces);

		void initKnightMoves();
		void initKingMoves();
		void initPawnAttacks();
		void initRookRays();
		void initBishopRays();
		void initAttackTables();
		void initDirectionTable();

	public:
		IndexSet getBitList(uint64_t bb);
		static int positionToNum(chess::Position pos);
		static chess::Position numToPosition(int sq);
		Bitboard(); // default constructor

		Bitboard(chess::VectorBoard &board, chess::Color side);
		void loadBitboard(chess::VectorBoard& board, chess::Color side);
		void printBitboard() const;
		void pBitboard(uint64_t allPieces);
		int getKingIndex(Color color)const;
		int getIndex(uint64_t bb) const;
		bool canEnPassant(Move move);

		//uint64_t attackersToKing(Color color);
		std::pair<uint64_t, uint64_t> getCheckMask(int kingSq, Color color, uint64_t attackers);
		uint64_t attackersToKing(Color color);
		uint64_t controledSquares(Color color);
		uint64_t rayBetween(int kingSq, int attackerSq);
		bool canCastle(bool kingSide, chess::Position fromPos);
		MoveList generateKingMovesOnly(Color side);
		bool isCheck(Color color) ;
		std::pair<IndexSet, std::array<uint64_t, 64>> getPinnedPieces();


		IndexSet getAllQueenMoves(int sq, chess::Color color);
		IndexSet getAllRookMoves(int sq, chess::Color color);
		IndexSet getAllBishopMoves(int sq, chess::Color color);
		IndexSet getAllKingMoves(int sq, chess::Color color);
		IndexSet getAllKnightMoves(int sq, chess::Color color);

		void generatePawnMoves(MoveList moves);
	};
}