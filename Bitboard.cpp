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
	Bitboard::Bitboard(chess::VectorBoard& board, chess::Color side) {
		initAttackTables();
		loadBitboard(board,side);
	}
	chess::Position Bitboard::numToPosition(int sq) {
		int row = 7 - (sq / 8);
		int col = sq - (7 - row) * 8;
		return chess::Position(row, col);
	}
	int  Bitboard::positionToNum(chess::Position pos) {
		return (7 - pos.row) * 8 + pos.column;
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

		if (color == Color::Black)
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
	std::pair<uint64_t, uint64_t> Bitboard::getCheckMask(int kingSq, Color color, uint64_t attackers) {


		uint64_t enemyRooks = (color == Color::Black) ? whiteRooks : blackRooks;
		uint64_t enemyBishops = (color == Color::Black) ? whiteBishops : blackBishops;
		uint64_t enemyQueens = (color == Color::Black) ? whiteQueens : blackQueens;

		// Occupancy of all pieces
		uint64_t occ = whitePieces | blackPieces;

		uint64_t checkMask = 0ULL;
		int attackerSq = getIndex(attackers);
		uint64_t attackerBB = 1ULL << attackerSq;

		// A friendly piece can capture the attacker
		checkMask |= attackerBB;

		// If it's a slider, a friendly piece can block the path
		if (attackerBB & (enemyBishops | enemyRooks | enemyQueens)) {
			checkMask |= rayBetween(kingSq, attackerSq);
		}

		// --- Calculate Controlled Mask (Where King cannot go) ---
		uint64_t controledMask = 0ULL;

		// IMPORTANT: Remove King from occupancy so sliders see "through" him
		// to cover squares like e2 when King is on e1 and Rook on e8.
		uint64_t occNoKing = occ ^ (1ULL << kingSq);

		if (attackerBB & (enemyRooks | enemyQueens)) {
			// Use Magic Bitboard lookup with OccNoKing
			controledMask |= rookAttacksWithBlockers(attackerSq, occNoKing);
		}

		if (attackerBB & (enemyBishops | enemyQueens)) {
			// Use Magic Bitboard lookup with OccNoKing
			controledMask |= bishopAttacksWithBlockers(attackerSq, occNoKing);
		}

		// Note: If attacker is Knight/Pawn, controledMask is just their static attack
		// (You might want to handle that else-if here for completeness, 
		// though the King step-logic usually handles static attacks separately).

		return { checkMask, controledMask };
	}
	bool Bitboard::canEnPassant(Move move) {
		uint64_t kingBB = sideToMove == Color::White ? whiteKings : blackKings;
		uint64_t enemyPawns = sideToMove == Color::Black ? whitePawns : blackPawns;
		uint64_t friendlyPawns = sideToMove == Color::White ? whitePawns : blackPawns;
		uint64_t enemyKnights = sideToMove == Color::Black ? whiteKnights : blackKnights;
		uint64_t enemyKings = sideToMove == Color::Black ? whiteKings : blackKings;
		uint64_t enemyRooks = sideToMove == Color::Black ? whiteRooks : blackRooks;
		uint64_t enemyBishops = sideToMove == Color::Black ? whiteBishops : blackBishops;
		uint64_t enemyQueens = sideToMove == Color::Black ? whiteQueens : blackQueens;
		uint64_t allMyPieces = allPieces;
		//pBitboard(~1ULL);
		uint64_t myPawnMask = (1ULL << move.getFromPos().getNumberIndex());
		friendlyPawns=(myPawnMask^friendlyPawns);
		allMyPieces = (myPawnMask ^ allMyPieces);
		myPawnMask = (1ULL << move.getToPos().getNumberIndex());
		allMyPieces = (myPawnMask | allMyPieces);
		friendlyPawns = (myPawnMask | friendlyPawns);

		chess::Position enemyPawnPosition = chess::Position(move.getFromPos().row, move.getToPos().column);
		uint64_t enemyPawnMask = (1ULL << enemyPawnPosition.getNumberIndex());
		enemyPawns = (enemyPawnMask ^ enemyPawns);
		allMyPieces = (enemyPawnMask ^ allMyPieces);
		
		return isAttacked(lsb(kingBB),sideToMove,enemyPawns,enemyKnights,enemyKings,enemyRooks,enemyBishops,enemyQueens,allMyPieces);
		
	}
	bool Bitboard::canCastle(bool kingSide,chess::Position fromPos) {

		int rookPos = (kingSide) ? 7 : 0;
		if (sideToMove == Color::Black) {
			rookPos += 56;
		}
		uint64_t castleRay = rayBetween(fromPos.getNumberIndex(), rookPos);
		if (castleRay & allPieces) return false;

		if (!kingSide) {
			castleRay=(castleRay^(1ULL << (rookPos + 1)));
		}
		if (castleRay & controledSquares(!sideToMove)) return false;

		return true;
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
		return isAttacked(kingSquare, color, enemyPawns, enemyKnights, enemyKings, enemyRooks, enemyBishops, enemyQueens, allPieces);

	}
	bool Bitboard::isAttacked(int square,Color color, uint64_t enemyPawns, uint64_t enemyKnights, uint64_t enemyKings, uint64_t enemyRooks, uint64_t enemyBishops, uint64_t enemyQueens, uint64_t allMyPieces) {
		// Knight check 
		if (knightMoves[square] & enemyKnights) return true;

		if (color == Color::White) {
			if (pawnAttacksWhite[square] & enemyPawns) return true;
		}
		else {

			if (pawnAttacksBlack[square] & enemyPawns) return true;
		}
		// Pawn check (black pawns attack downwards) 

		// King check (rare but possible in eval) 
		if (kingMoves[square] & enemyKings) return true;

		// Sliding pieces 
		// Rook / queen attacks 
		if (rookAttacksWithBlockers(square, allMyPieces) & (enemyRooks | enemyQueens)) return true;
		// Bishop / queen attacks 
		if (bishopAttacksWithBlockers(square, allMyPieces) & (enemyBishops | enemyQueens)) return true;

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
		uint64_t pinnedPiecesMap = 0ULL;
		std::array<uint64_t, 64> pinMask;
		pinMask.fill(0ULL);

		// Setup pieces based on side to move
		uint64_t kings = (sideToMove == Color::White) ? whiteKings : blackKings;
		uint64_t friendlyPieces = (sideToMove == Color::White) ? whitePieces : blackPieces;
		uint64_t enemyPieces = (sideToMove == Color::White) ? blackPieces : whitePieces;
		uint64_t enemyRooks = (sideToMove == Color::White) ? blackRooks : whiteRooks;
		uint64_t enemyBishops = (sideToMove == Color::White) ? blackBishops : whiteBishops;
		uint64_t enemyQueens = (sideToMove == Color::White) ? blackQueens : whiteQueens;

		int kingSq = lsb(kings);

		// N, S, E, W, NE, SW, NW, SE
		static const int dir[8] = { 8, -8, 1, -1, 9, -9, 7, -7 };

		for (int d = 0; d < 8; d++) {
			int sq = kingSq;
			uint64_t rayMask = 0ULL;
			int blockerSq = -1; // Use int for index, clearer than checking uint64 bit

			while (true) {
				int prevSq = sq;
				sq += dir[d];

				// 1. Check Vertical Bounds
				if (sq < 0 || sq >= 64) break;

				// 2. Check Horizontal Wrap (The Logic Fix)
				// If the file distance between current and prev square is > 1, we wrapped.
				int currentFile = sq & 7;
				int prevFile = prevSq & 7;
				if (std::abs(currentFile - prevFile) > 1) break;

				uint64_t bit = 1ULL << sq;
				rayMask |= bit; // Add square to the ray

				if (friendlyPieces & bit) {
					if (blockerSq == -1) {
						blockerSq = sq; // Found the first friendly piece (potential pin)
					}
					else {
						break; // Second friendly piece found, no pin possible
					}
				}
				else if (enemyPieces & bit) {
					// If we hit an enemy, check if it's the right type to pin
					if (blockerSq != -1) {
						bool isDiagonal = (d >= 4); // Indices 4-7 are diagonals

						uint64_t attackers = isDiagonal ? (enemyBishops | enemyQueens)
							: (enemyRooks | enemyQueens);

						if (bit & attackers) {
							// We found a pinner!
							pinnedPiecesMap |= (1ULL << blockerSq);
							pinMask[blockerSq] = rayMask;
						}
					}
					break; // Hit an enemy piece, stop scanning this ray
				}
			}
		}
		return { getBitList(pinnedPiecesMap), pinMask };
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