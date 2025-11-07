#pragma once
#include<iostream>
#include <array>          // ✅ MUST be first or at least before using std::array
#include <unordered_set>

#include<string>
#include "Position.h";
#include "Square.h";
#include "Move.h";
#include<vector>
namespace chess {
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



		static const uint64_t FILE_A = 0x0101010101010101ULL;
		static const uint64_t FILE_H = 0x8080808080808080ULL;
		uint64_t rookAttacksWithBlockers(int sq, uint64_t occ);
		uint64_t bishopAttacksWithBlockers(int sq, uint64_t occ);
		void initKnightMoves();
		void initKingMoves();
		void initPawnAttacks();
		void initRookRays();
		void initBishopRays();
		void initAttackTables();
		void initDirectionTable();

	public:
		std::unordered_set<int> getBitList(uint64_t bb);
		static int positionToNum(chess::Position pos);
		static chess::Position numToPosition(int sq);
		Bitboard(); // default constructor

		Bitboard(chess::VectorBoard &board);
		void loadBitboard(chess::VectorBoard& board);
		void printBitboard() const;
		int getKingIndex(Color color)const;
		int getIndex(uint64_t bb) const;

		//uint64_t attackersToKing(Color color);
		std::pair<uint64_t, uint64_t> getCheckMask(int kingSq, Color color, uint64_t attackers);
		uint64_t attackersToKing(Color color);
		uint64_t controledSquares(Color color);
		uint64_t rayBetween(int kingSq, int attackerSq);
		std::vector<Move> generateKingMovesOnly(Color side);
		bool isCheck(Color color) ;
		std::pair<std::unordered_set<int>, std::array<uint64_t, 64>> getPinnedPieces();


		std::unordered_set<int> getAllQueenMoves(int sq, chess::Color color);
		std::unordered_set<int> getAllRookMoves(int sq, chess::Color color);
		std::unordered_set<int> getAllBishopMoves(int sq, chess::Color color);
		std::unordered_set<int> getAllKingMoves(int sq, chess::Color color);
		std::unordered_set<int> getAllKnightMoves(int sq, chess::Color color);

		void generatePawnMoves(std::vector<Move> moves);
	};
}