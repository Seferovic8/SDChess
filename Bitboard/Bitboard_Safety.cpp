#include "Bitboard.h"

namespace chess {
	
	bool Bitboard::isAttacked(int square, Color color, uint64_t enemyPawns, uint64_t enemyKnights, uint64_t enemyKings, uint64_t enemyRooks, uint64_t enemyBishops, uint64_t enemyQueens, uint64_t allMyPieces) {
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
	bool Bitboard::isCheck(Color color) {
		uint64_t kingBB = color == Color::White ? whiteKings : blackKings;
		uint64_t enemyPawns = color == Color::Black ? whitePawns : blackPawns;
		uint64_t enemyKnights = color == Color::Black ? whiteKnights : blackKnights;
		uint64_t enemyKings = color == Color::Black ? whiteKings : blackKings;
		uint64_t enemyRooks = color == Color::Black ? whiteRooks : blackRooks;
		uint64_t enemyBishops = color == Color::Black ? whiteBishops : blackBishops;
		uint64_t enemyQueens = color == Color::Black ? whiteQueens : blackQueens;

		//std::cout << "AA";
		int kingSquare = lsb(kingBB); // king square 
		//pBitboard(kingBB);
		return isAttacked(kingSquare, color, enemyPawns, enemyKnights, enemyKings, enemyRooks, enemyBishops, enemyQueens, allPieces);

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
		uint64_t occNoKing = occ ^ (1ULL << kingSq);

		if (attackerBB & (enemyRooks | enemyQueens)) {
			// Use Magic Bitboard lookup with OccNoKing
			controledMask |= rookAttacksWithBlockers(attackerSq, occNoKing);
		}

		if (attackerBB & (enemyBishops | enemyQueens)) {
			// Use Magic Bitboard lookup with OccNoKing
			controledMask |= bishopAttacksWithBlockers(attackerSq, occNoKing);
		}
		return { checkMask, controledMask };
	}

	std::pair<IndexSet, std::array<uint64_t, 64>> Bitboard::getPinnedPieces() {
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
			int blockerSq = -1;

			while (true) {
				int prevSq = sq;
				sq += dir[d];

				if (sq < 0 || sq >= 64) break;
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
						break; 
					}
				}
				else if (enemyPieces & bit) {
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
					break;
				}
			}
		}
		return { getBitList(pinnedPiecesMap), pinMask };
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
		friendlyPawns = (myPawnMask ^ friendlyPawns);
		allMyPieces = (myPawnMask ^ allMyPieces);
		myPawnMask = (1ULL << move.getToPos().getNumberIndex());
		allMyPieces = (myPawnMask | allMyPieces);
		friendlyPawns = (myPawnMask | friendlyPawns);

		chess::Position enemyPawnPosition = chess::Position(move.getFromPos().row, move.getToPos().column);
		uint64_t enemyPawnMask = (1ULL << enemyPawnPosition.getNumberIndex());
		enemyPawns = (enemyPawnMask ^ enemyPawns);
		allMyPieces = (enemyPawnMask ^ allMyPieces);

		return isAttacked(lsb(kingBB), sideToMove, enemyPawns, enemyKnights, enemyKings, enemyRooks, enemyBishops, enemyQueens, allMyPieces);

	}
	bool Bitboard::canCastle(bool kingSide, chess::Position fromPos) {

		int rookPos = (kingSide) ? 7 : 0;
		if (sideToMove == Color::Black) {
			rookPos += 56;
		}
		uint64_t castleRay = rayBetween(fromPos.getNumberIndex(), rookPos);
		if (castleRay & allPieces) return false;

		if (!kingSide) {
			castleRay = (castleRay ^ (1ULL << (rookPos + 1)));
		}
		if (castleRay & controledSquares(!sideToMove)) return false;

		return true;
	}
}