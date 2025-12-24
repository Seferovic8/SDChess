#include "Board.h"

namespace chess {
	void Board::generatePawnMoves(int sq, MoveList& moves) {
		// 1. Get Bitboard State to identify piece and occupancy
		StateBitboard state = bitboard.getBitboardState();
		uint64_t fromMask = 1ULL << sq;
		PieceType pt = PieceType::None;
		Color color = Color::White;

		// Identify Piece Type and Color using bitmasks
		if (state.whitePawns & fromMask) { pt = PieceType::Pawn; color = Color::White; }
		else if (state.blackPawns & fromMask) { pt = PieceType::Pawn; color = Color::Black; }
		else if (state.whiteKings & fromMask) { pt = PieceType::King; color = Color::White; }
		else if (state.blackKings & fromMask) { pt = PieceType::King; color = Color::Black; }

		if (pt == PieceType::None) return;


		// Reconstruct occupancy bitboards locally
		uint64_t whitePieces = state.whitePawns | state.whiteKnights | state.whiteBishops | state.whiteRooks | state.whiteQueens | state.whiteKings;
		uint64_t blackPieces = state.blackPawns | state.blackKnights | state.blackBishops | state.blackRooks | state.blackQueens | state.blackKings;
		uint64_t allPieces = whitePieces | blackPieces;
		uint64_t enemyPieces = (color == Color::White) ? blackPieces : whitePieces;

		// Direction and Rank logic
		// Based on Bitboard.cpp: White moves +8 (index increases), Black moves -8.
		int forward = (color == Color::White) ? 8 : -8;
		int captureLeft = (color == Color::White) ? 7 : -9;  // +7 is NE (if +8 is N) or similar diagonal
		int captureRight = (color == Color::White) ? 9 : -7;

		int startRankIdx = (color == Color::White) ? 6 : 1; // Bitboard rank index (0-7)
		int promoRankIdx = (color == Color::White) ? 7 : 0;

		// --- Single Push ---
		int targetSq = sq + forward;
		if (targetSq >= 0 && targetSq < 64 && !(allPieces & (1ULL << targetSq))) {
			int targetRank = targetSq / 8;

			if (targetRank == promoRankIdx) {
				moves.push_back(Move(sq, targetSq, false, Castling::NONE, chess::PieceType::Queen));
				moves.push_back(Move(sq, targetSq, false, Castling::NONE, chess::PieceType::Rook));
				moves.push_back(Move(sq, targetSq, false, Castling::NONE, chess::PieceType::Bishop));
				moves.push_back(Move(sq, targetSq, false, Castling::NONE, chess::PieceType::Knight));

			}
			else {
				moves.push_back(Move(sq, targetSq));

				// --- Double Push ---
				// Only possible if Single Push was valid and we are on start rank
				if ((7 - (sq / 8)) == startRankIdx) {
					int doubleTarget = sq + (forward * 2);
					if (!(allPieces & (1ULL << doubleTarget))) {
						moves.push_back(Move(sq, doubleTarget));
					}
				}
			}
		}

			// --- Captures ---
			int captures[] = { captureLeft, captureRight };
			for (int capOffset : captures) {
				int capSq = sq + capOffset;
				if (capSq < 0 || capSq >= 64) continue;

				// Check file wrap-around (abs(file1 - file2) must be 1)
				if (abs((sq % 8) - (capSq % 8)) > 1) continue;

				if (enemyPieces & (1ULL << capSq)) {
					int targetRank = capSq / 8;

					if (targetRank == promoRankIdx) {
						moves.push_back(Move(sq, capSq, false, Castling::NONE, chess::PieceType::Queen));
						moves.push_back(Move(sq, capSq, false, Castling::NONE, chess::PieceType::Rook));
						moves.push_back(Move(sq, capSq, false, Castling::NONE, chess::PieceType::Bishop));
						moves.push_back(Move(sq, capSq, false, Castling::NONE, chess::PieceType::Knight));
					}
					else {
						moves.push_back(Move(sq, capSq));
					}
				}
			}

			// --- En Passant ---
			// Requires checking History for the last move
			if (!history.empty() ) {
				// En Passant happens on specific ranks (White: logic rank 3 -> idx ~32?, Black: logic rank 4)
				// We check if an enemy pawn is adjacent to us (sq-1 or sq+1)

				int adjOffsets[] = { -1, 1 };
				for (int offset : adjOffsets) {
					int adjSq = sq + offset;
					if (adjSq < 0 || adjSq >= 64) continue;

					// File wrap check for adjacency
					if (abs((sq % 8) - (adjSq % 8)) != 1) continue;

					uint64_t adjMask = (1ULL << adjSq);

					// Must have an enemy pawn there
					uint64_t relevantEnemyPawns = (color == Color::White) ? state.blackPawns : state.whitePawns;

					if (relevantEnemyPawns & adjMask) {
						// Check if that pawn just moved double
						auto lastMove = history.back().move;
						int lastTo = lastMove.getToSq();
						int lastFrom = lastMove.getFromSq();

						if (lastTo == adjSq && abs(lastFrom - lastTo) == 16) {
							// Valid En Passant Target
							int destSq = adjSq + forward; // Move to the square *behind* the pawn

							auto enPassantMove = Move(sq, destSq, true);

							// Original logic used !canEnPassant to validate safety
							if (!bitboard.canEnPassant(enPassantMove)) {
								moves.push_back(enPassantMove);
							}
						}
					}
				}
			
		}
	}
	MoveList Board::getAllPseudoLegalMoves() {
		MoveList allMoves;
		StateBitboard state = bitboard.getBitboardState();

		uint64_t myKnights = sideToMove == Color::White ? state.whiteKnights : state.blackKnights;
		uint64_t myKings = sideToMove == Color::White ? state.whiteKings : state.blackKings;
		uint64_t myPawns = sideToMove == Color::White ? state.whitePawns : state.blackPawns;
		uint64_t myRooks = sideToMove == Color::White ? state.whiteRooks : state.blackRooks;
		uint64_t myBishops = sideToMove == Color::White ? state.whiteBishops : state.blackBishops;
		uint64_t myQueens = sideToMove == Color::White ? state.whiteQueens : state.blackQueens;
		while (myKnights) {
			int fromSq = Bitboard::lsb(myKnights);   // index of least-significant 1-bit
			myKnights &= myKnights - 1;       // remove that bit
			auto knightMoves = bitboard.getAllKnightMoves(fromSq, sideToMove);
			while (knightMoves) {
				int toSq = Bitboard::lsb(knightMoves);   // index of least-significant 1-bit
				knightMoves &= knightMoves - 1;
				allMoves.push_back(Move(fromSq, toSq));
			}
		}
		while (myRooks) {
			int fromSq = Bitboard::lsb(myRooks);   // index of least-significant 1-bit
			myRooks &= myRooks - 1;       // remove that bit
			auto rookMoves = bitboard.getAllRookMoves(fromSq, sideToMove);
			while (rookMoves) {
				int toSq = Bitboard::lsb(rookMoves);   // index of least-significant 1-bit
				rookMoves &= rookMoves - 1;
				allMoves.push_back(Move(fromSq, toSq));
			}
		}
		while (myBishops) {
			int fromSq = Bitboard::lsb(myBishops);   // index of least-significant 1-bit
			myBishops &= myBishops - 1;       // remove that bit
			auto bishopMoves = bitboard.getAllBishopMoves(fromSq, sideToMove);
			while (bishopMoves) {
				int toSq = Bitboard::lsb(bishopMoves);   // index of least-significant 1-bit
				bishopMoves &= bishopMoves - 1;
				allMoves.push_back(Move(fromSq, toSq));
			}
		}
		while (myQueens) {
			int fromSq = Bitboard::lsb(myQueens);   // index of least-significant 1-bit
			myQueens &= myQueens - 1;       // remove that bit
			auto queensMoves = bitboard.getAllQueenMoves(fromSq, sideToMove);
			while (queensMoves) {
				int toSq = Bitboard::lsb(queensMoves);   // index of least-significant 1-bit
				queensMoves &= queensMoves - 1;
				allMoves.push_back(Move(fromSq, toSq));
			}
		}
		while (myKings) {

			int fromSq = Bitboard::lsb(myKings);   // index of least-significant 1-bit

			int piecePos = (sideToMove == Color::White) ? 7 : 0;
			bool kingSide = (sideToMove == Color::White) ? castling.whiteKing : castling.blackKing;
			bool queenSide = (sideToMove == Color::White) ? castling.whiteQueen : castling.blackQueen;

			chess::Position fromPos = chess::Position(fromSq);
			if (queenSide) {
				if (bitboard.canCastle(false, fromSq)) {
					allMoves.push_back(Move(fromPos.row, fromPos.column, piecePos, 2, false, Castling::Queen));
				}
			}
			if (kingSide) {
				if (bitboard.canCastle(true, fromSq)) {
					allMoves.push_back(Move(fromPos.row, fromPos.column, piecePos, 6, false, Castling::King));
				}
			}
			myKings &= myKings - 1;       // remove that bit
			auto kingMoves = bitboard.getAllKingMoves(fromSq, sideToMove);
			while (kingMoves) {
				int toSq = Bitboard::lsb(kingMoves);   // index of least-significant 1-bit
				kingMoves &= kingMoves - 1;
				allMoves.push_back(Move(fromSq, toSq));
			}
		}

		while (myPawns) {
			// Get the index of the first piece (0-63)
			int sq = Bitboard::lsb(myPawns);
			
			generatePawnMoves(sq, allMoves);

			myPawns &= (myPawns - 1);

		}


		return allMoves;
	}



	MoveList Board::getAllLegalMoves() {
		MoveList legalMoves;
		//bitboard.loadBitboard(board, sideToMove);
		uint64_t controled = bitboard.controledSquares(!sideToMove);
		int kingIndex = bitboard.getKingIndex(sideToMove);

		uint64_t attackers = bitboard.attackersToKing(sideToMove);
		//if (this->isCheckMate()) {
		//	return legalMoves;
		//}
		MoveList pseudoMoves = getAllPseudoLegalMoves();
		if (this->isCheck()) {
			int numAttackers = bitboard.getBitList(attackers).size();
			if (numAttackers >= 2) {
				pseudoMoves = bitboard.generateKingMovesOnly(kingIndex, sideToMove);
			}
		}
		auto pinned = bitboard.getPinnedPieces();
		IndexSet pinnedPieces = pinned.first;
		auto pinMask = pinned.second;

		for (Move mv : pseudoMoves) {
			int fromPosNum = mv.getFromSq();
			int toPosNum = mv.getToSq();

			if (this->isCheck()) {
				std::pair<uint64_t, uint64_t> mask = bitboard.getCheckMask(kingIndex, sideToMove, attackers);
				uint64_t checkMask = mask.first;
				uint64_t controledMask = mask.second;
				if (bitboard.getPieceType(fromPosNum)==chess::PieceType::King) {

					if (!(controled & (1ULL << toPosNum))) {
						if (!(controledMask & (1ULL << toPosNum))) {
							legalMoves.push_back(mv);
						}
					}
				}
				else {
					// --- NON-KING LOGIC ---
					bool resolvesCheck = (checkMask & (1ULL << toPosNum));

					bool satisfiesPin = true;

					if (pinnedPieces.count(fromPosNum)) {
						IndexSet allowedPinMoves = bitboard.getBitList(pinMask[fromPosNum]);

						if (!allowedPinMoves.count(toPosNum)) {
							satisfiesPin = false; // It moves off the pin ray
						}
					}

					if (resolvesCheck && satisfiesPin) {
						legalMoves.push_back(mv);
					}
				}
			}
			else {

				if (pinnedPieces.count(fromPosNum)) {
					IndexSet allowedPinMoves = bitboard.getBitList(pinMask[fromPosNum]);
					if (allowedPinMoves.count(toPosNum)) {
						legalMoves.push_back(mv);
					}
				}
				else if (bitboard.getPieceType(fromPosNum) == chess::PieceType::King) {
					if (!(controled & (1ULL << toPosNum))) {
						legalMoves.push_back(mv);

					}
				}
				else {
					legalMoves.push_back(mv);

				}
			}

		}
		return legalMoves;

	}
	MoveList Board::getAllLegalCaptures() {
		MoveList allMoves = getAllLegalMoves();
		MoveList captureMoves;
		for (Move mv : allMoves) {
			auto pos = mv.getToPos();
			if (board[pos.row][pos.column].hasPiece()) {
				captureMoves.push_back(mv);
			}
		}
		return captureMoves;
	}
	void Board::rookCastling(chess::Position pos, Color pieceColor) {
		if (pos.column == 0) {
			if (pieceColor == Color::White) {
				castling.whiteQueen = false;
			}
			else {
				castling.blackQueen = false;
			}
		}
		else if (pos.column == 7) {
			if (pieceColor == Color::White) {
				castling.whiteKing = false;
			}
			else {
				castling.blackKing = false;
			}
		}
	}
	bool Board::isCheck() {

		if (check == Check::Undefined) {
			if (bitboard.isCheck(sideToMove)) {
				check = Check::Yes;
				return true;
			}
			check = Check::No;
			return false;
		}
		else {
			if (check == Check::Yes) {
				return true;
			}
			return false;
		}
	}
	bool Board::isCheckMate() {
		auto moves = getAllLegalMoves();
		if (moves.count == 0) {
			return isCheck();
		}
		return false;
	}
}