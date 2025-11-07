#include<iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <array>

#include "Bitboard.h"
#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace chess {
	Bitboard::Bitboard() {
		initAttackTables();

	}
	Bitboard::Bitboard(chess::VectorBoard& board) {
		initAttackTables();
		loadBitboard(board);
	}
	chess::Position Bitboard::numToPosition(int sq) {
		int row = 7 - (sq / 8);
		int col = sq - (7 - row) * 8;
		return chess::Position(row, col);
	}
	int  Bitboard::positionToNum(chess::Position pos) {
		return (7 - pos.row) * 8 + pos.column;
	}
	void Bitboard::loadBitboard(chess::VectorBoard& board) {
		whitePawns = whiteKnights = whiteBishops = whiteRooks = whiteQueens = whiteKings = 0ULL;
		blackPawns = blackKnights = blackBishops = blackRooks = blackQueens = blackKings = 0ULL;
		for (int r = 0; r < 8; r++) {
			for (int c = 0; c < 8; c++) {
				if (!board[r][c].hasPiece())
					continue;

				auto piece = board[r][c].getPiece();
				int sq = (7 - r) * 8 + c; // convert to 0–63

				if (piece->getColor() == Color::White) {
					if (piece->getPieceType() == PieceType::Pawn)   whitePawns |= (1ULL << sq);
					else if (piece->getPieceType() == PieceType::Knight) whiteKnights |= (1ULL << sq);
					else if (piece->getPieceType() == PieceType::Bishop) whiteBishops |= (1ULL << sq);
					else if (piece->getPieceType() == PieceType::Rook)   whiteRooks |= (1ULL << sq);
					else if (piece->getPieceType() == PieceType::Queen)  whiteQueens |= (1ULL << sq);
					else if (piece->getPieceType() == PieceType::King)   whiteKings |= (1ULL << sq);
				}
				else { // Black pieces
					if (piece->getPieceType() == PieceType::Pawn)   blackPawns |= (1ULL << sq);
					else if (piece->getPieceType() == PieceType::Knight) blackKnights |= (1ULL << sq);
					else if (piece->getPieceType() == PieceType::Bishop) blackBishops |= (1ULL << sq);
					else if (piece->getPieceType() == PieceType::Rook)   blackRooks |= (1ULL << sq);
					else if (piece->getPieceType() == PieceType::Queen)  blackQueens |= (1ULL << sq);
					else if (piece->getPieceType() == PieceType::King)   blackKings |= (1ULL << sq);
				}
			}
		}

		// Combined maps
		whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKings;
		blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKings;
		allPieces = whitePieces | blackPieces;
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
	void pBitboard(uint64_t allPieces)
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
	inline int lsb(uint64_t bb) {
#ifdef _MSC_VER
		unsigned long index;
		_BitScanForward64(&index, bb);
		return (int)index;
#else
		return __builtin_ctzll(bb);
#endif
	}

	int Bitboard::getKingIndex(Color color) const {
		uint64_t kings = (color == Color::White) ? whiteKings : blackKings;
		return lsb(kings);
	}
	uint64_t Bitboard::attackersToKing(Color color) {
		uint64_t kingBB = color == Color::White ? whiteKings : blackKings;
		uint64_t enemyPawns = color == Color::Black ? whitePawns : blackPawns;
		uint64_t enemyKnights = color == Color::Black ? whiteKnights : blackKnights;
		uint64_t enemyKings = color == Color::Black ? whiteKings : blackKings;
		uint64_t enemyRooks = color == Color::Black ? whiteRooks : blackRooks;
		uint64_t enemyBishops = color == Color::Black ? whiteBishops : blackBishops;
		uint64_t enemyQueens = color == Color::Black ? whiteQueens : blackQueens;

		int ks = lsb(kingBB);
		uint64_t occ = allPieces;

		uint64_t attackers = 0ULL;

		if (color == Color::White)
			attackers |= pawnAttacksBlack[ks] & enemyPawns; // black pawns attack down
		else
			attackers |= pawnAttacksWhite[ks] & enemyPawns; // white pawns attack up
		attackers |= knightMoves[ks] & enemyKnights;
		attackers |= kingMoves[ks] & enemyKings;
		attackers |= rookAttacksWithBlockers(ks, occ) & (enemyRooks | enemyQueens);
		//attackers |= (rookAttacksWithBlockers(ks, occ));
		attackers |= bishopAttacksWithBlockers(ks, occ) & (enemyBishops | enemyQueens);

		return attackers;
	}
	int Bitboard::getIndex(uint64_t bb)const {
		return lsb(bb);
	}
	std::pair<uint64_t,uint64_t> Bitboard::getCheckMask(int kingSq, Color color, uint64_t attackers) {
		uint64_t enemyPawns = color == Color::Black ? whitePawns : blackPawns;
		uint64_t enemyKnights = color == Color::Black ? whiteKnights : blackKnights;
		uint64_t enemyKings = color == Color::Black ? whiteKings : blackKings;
		uint64_t enemyRooks = color == Color::Black ? whiteRooks : blackRooks;
		uint64_t enemyBishops = color == Color::Black ? whiteBishops : blackBishops;
		uint64_t enemyQueens = color == Color::Black ? whiteQueens : blackQueens;
		uint64_t checkMask = 0ULL;
		int attackerSq = getIndex(attackers);
		uint64_t attackerBB = 1ULL << attackerSq;

		// King can always capture attacker if legal
		checkMask |= attackerBB;

		// Sliding piece? then add block squares
		if (attackerBB & (enemyBishops | enemyRooks | enemyQueens)) {
			checkMask |= rayBetween(kingSq, attackerSq);
		}
		uint64_t controledMask = 0LL;
		if (attackerBB & enemyRooks) {
			controledMask = rookRays[attackerSq];
		}
		else if (attackerBB & enemyBishops) {

			controledMask = bishopRays[attackerSq];
		}
		else if (attackerBB & enemyQueens) {

			controledMask = rookRays[attackerSq] | bishopRays[attackerSq];
		}
		if (!(controledMask & (1ULL << 26))) {
			pBitboard((controledMask & (1ULL << 26)));

		}
		return {checkMask,controledMask};
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

	uint64_t Bitboard::controledSquares(Color color) {
		uint64_t occ = allPieces;

		uint64_t pawns = (color == Color::White) ? whitePawns : blackPawns;
		uint64_t knights = (color == Color::White) ? whiteKnights : blackKnights;
		uint64_t kings = (color == Color::White) ? whiteKings : blackKings;
		uint64_t rooks = (color == Color::White) ? whiteRooks : blackRooks;
		uint64_t bishops = (color == Color::White) ? whiteBishops : blackBishops;
		uint64_t queens = (color == Color::White) ? whiteQueens : blackQueens;

		uint64_t attacks = 0ULL;

		// Pawn attacks
		uint64_t pawnsBB = pawns;
		while (pawnsBB) {
			int sq = lsb(pawnsBB);
			pawnsBB &= pawnsBB - 1;
			attacks |= (color == Color::White) ? pawnAttacksWhite[sq]
				: pawnAttacksBlack[sq];
		}

		// Knight attacks
		uint64_t knBB = knights;
		while (knBB) {
			int sq = lsb(knBB);
			knBB &= knBB - 1;
			attacks |= knightMoves[sq];
		}

		// King attacks
		attacks |= kingMoves[lsb(kings)];

		// Sliding pieces
		uint64_t bb = rooks | queens;
		while (bb) {
			int sq = lsb(bb);
			bb &= bb - 1;
			attacks |= rookAttacksWithBlockers(sq, occ);
		}

		bb = bishops | queens;
		while (bb) {
			int sq = lsb(bb);
			bb &= bb - 1;
			attacks |= bishopAttacksWithBlockers(sq, occ);
		}
		return attacks;
	}

	std::vector<Move> Bitboard::generateKingMovesOnly(Color side) {
		std::vector<Move> moves;
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

			moves.push_back(Move(numToPosition(from), numToPosition(to)));   // your Move struct
		}
		return moves;
	}

	bool Bitboard::isCheck(Color color) {
		uint64_t kingBB = color == Color::White ? whiteKings : blackKings;
		uint64_t enemyPawns = color == Color::Black ? whitePawns : blackPawns;
		uint64_t enemyKnights = color == Color::Black ? whiteKnights : blackKnights;
		uint64_t enemyKings = color == Color::Black ? whiteKings : blackKings;
		uint64_t enemyRooks = color == Color::Black ? whiteRooks : blackRooks;
		uint64_t enemyBishops = color == Color::Black ? whiteBishops : blackBishops;
		uint64_t enemyQueens = color == Color::Black ? whiteQueens : blackQueens;


		int kingSquare = lsb(kingBB); // king square 
		// Knight check 
		if (knightMoves[kingSquare] & enemyKnights) return true;

		if (color == Color::White) {
			if (pawnAttacksBlack[kingSquare] & enemyPawns) return true;
		}
		else {

			if (pawnAttacksWhite[kingSquare] & enemyPawns) return true;
		}
		// Pawn check (black pawns attack downwards) 

		// King check (rare but possible in eval) 
		if (kingMoves[kingSquare] & enemyKings) return true;

		// Sliding pieces 
		// Rook / queen attacks 
		if (rookAttacksWithBlockers(kingSquare, allPieces) & (enemyRooks | enemyQueens)) return true;
		// Bishop / queen attacks 
		if (bishopAttacksWithBlockers(kingSquare, allPieces) & (enemyBishops | enemyQueens)) return true;

		return false;

	}


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

	std::unordered_set<int> Bitboard::getBitList(uint64_t bb) {
		std::unordered_set<int> lista;
		while (bb) {
			int sq = lsb(bb);   // index of least-significant 1-bit
			bb &= bb - 1;       // remove that bit
			lista.insert(sq);
		}
		return lista;
	}

	std::unordered_set<int> Bitboard::getAllQueenMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return getBitList((rookAttacksWithBlockers(sq, allPieces) | bishopAttacksWithBlockers(sq, allPieces)) & ~myPieces);
	}
	std::unordered_set<int> Bitboard::getAllBishopMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return getBitList(bishopAttacksWithBlockers(sq, allPieces) & ~myPieces);
	}
	std::unordered_set<int> Bitboard::getAllRookMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return getBitList(rookAttacksWithBlockers(sq, allPieces) & ~myPieces);
	}
	std::unordered_set<int> Bitboard::getAllKnightMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return getBitList(knightMoves[sq] & ~myPieces);
	}
	std::unordered_set<int> Bitboard::getAllKingMoves(int sq, chess::Color color) {
		uint64_t myPieces = color == chess::Color::White ? whitePieces : blackPieces;
		return getBitList(kingMoves[sq] & ~myPieces);
	}
	std::pair<std::unordered_set<int>, std::array<uint64_t, 64>> Bitboard::getPinnedPieces() {
		auto pinnedPieces = 0ULL;
		//uint64_t pinMask[64];
		//memset(pinMask, 0, sizeof(pinMask));
		std::array<uint64_t, 64> pinMask{};
		pinMask.fill(0);

		int kingSq = lsb(whiteKings);
		static const int dir[8] = { 8,-8,1,-1,9,-9,7,-7 };

		for (int d = 0; d < 8; d++) {
			int sq = kingSq;
			uint64_t rayMask = 0ULL;
			uint64_t blocker = 0ULL;

			while (true) {
				sq += dir[d];

				// off board or wrap
				if (sq < 0 || sq >= 64) break;
				if ((dir[d] == 1 && sq % 8 == 0) ||
					(dir[d] == -1 && sq % 8 == 7) ||
					(dir[d] == 9 && sq % 8 == 0) ||
					(dir[d] == 7 && sq % 8 == 7)) break;

				uint64_t bit = 1ULL << sq;
				rayMask |= bit;

				// friendly piece
				if (whitePieces & bit) {
					if (!blocker) blocker = bit;
					else break; // second friendly → no pin here
				}
				// enemy piece
				else if (blackPieces & bit) {
					bool rookLine = (d < 4);
					bool bishopLine = (d >= 4);

					if (blocker) {
						if ((rookLine && (bit & (blackRooks | blackQueens))) ||
							(bishopLine && (bit & (blackBishops | blackQueens)))) {

							// ✅ We have a pinned piece
							pinnedPieces |= blocker;
							pinMask[lsb(blocker)] = rayMask;
						}
					}
					break;
				}
			}
		}
		//pBitboard(pinnedPieces);
		//pBitboard(pinMask[5 + (8 * 4)]);
		//pBitboard(pinMask[3 + (8 * 3)]);
		return { getBitList(pinnedPieces), pinMask };
	}

}
//void Bitboard::checkForPins() {
//	int kingSquare = lsb(whiteKings);
//	uint64_t kingPins = rookRays[kingSquare] | bishopRays[kingSquare];
//	std::cout << std::endl;

//	/*uint64_t clashingRooks = rookRays[kingSquare] & (blackRooks);
//	uint64_t clashingQueens = kingPins & (blackQueens);
//	uint64_t clashingBishop = bishopRays[kingSquare] & (blackBishops);

//	uint64_t pinnedWithBishops = (bishopAttacksWithBlockers(lsb(clashingBishop),allPieces) & (bishopAttacksWithBlockers(kingSquare, allPieces)))&allPieces;
//	uint64_t pinnedWithRooks = (rookAttacksWithBlockers(lsb(clashingRooks),allPieces) & (rookAttacksWithBlockers(kingSquare, allPieces)))&allPieces;

//	uint64_t queenAttacks = bishopAttacksWithBlockers(lsb(clashingQueens), allPieces) | rookAttacksWithBlockers(lsb(clashingQueens), allPieces);
//	uint64_t kingQueenAttacks = bishopAttacksWithBlockers(kingSquare, allPieces) | rookAttacksWithBlockers(kingSquare, allPieces);
//	uint64_t pinnedWithQueens = (queenAttacks & (kingQueenAttacks))&allPieces;

//	uint64_t pinned = pinnedWithBishops | pinnedWithQueens | pinnedWithRooks;*/
//	//pBitboard(bishopAttacksWithBlockers(kingSquare,allPieces));
//	pBitboard(pinned);
//	//getIndex(clashingBishop);

//}
//void Bitboard::generatePawnMoves(std::vector<Move>& moves) {
//	uint64_t pawns = whitePawns;

//	while (pawns) {
//		int from = lsb(pawns);
//		pawns &= pawns - 1;

//		uint64_t fromBB = 1ULL << from;
//		uint64_t movesMask = 0ULL;

//		// If pinned, restrict movement
//		if (pinnedPieces & fromBB)
//			; // moves must stay inside pinMask
//		else
//			; // moves unrestricted

//		int r = from / 8;

//		// forward push
//		int fwd = from + 8;
//		if (!(allPieces & (1ULL << fwd))) {
//			movesMask |= (1ULL << fwd);

//			// double push
//			if (r == 1 && !(allPieces & (1ULL << (from + 16))))
//				movesMask |= (1ULL << (from + 16));
//		}

//		// captures
//		uint64_t attacks = pawnAttacksWhite[from] & enemyPieces;
//		movesMask |= attacks;

//		// apply pin restriction if pawn pinned
//		if (pinnedPieces & fromBB)
//			movesMask &= pinMask[from];

//		// apply check mask (if in check, only these squares matter)
//		movesMask &= checkMask;

//		// add moves
//		while (movesMask) {
//			int to = lsb(movesMask);
//			movesMask &= movesMask - 1;

//			// promotions
//			if (to / 8 == 7) {
//				moves.push({ from,to,QUEEN });
//				moves.push({ from,to,ROOK });
//				moves.push({ from,to,BISHOP });
//				moves.push({ from,to,KNIGHT });
//			}
//			else {
//				moves.push({ from,to });
//			}
//		}
//	}
//}