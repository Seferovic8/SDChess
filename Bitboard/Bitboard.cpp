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
	void Bitboard::updateMaterial() {
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
}
