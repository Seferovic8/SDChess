#include "Board.h"

namespace chess {
	void Board::generatePieceMoves(chess::Position fromPos, MoveList& moves) {
		// 1. Get Bitboard State to identify piece and occupancy
		StateBitboard state = bitboard.getBitboardState();
		int sq = fromPos.getNumberIndex();
		uint64_t fromMask = 1ULL << sq;

		PieceType pt = PieceType::None;
		Color color = Color::White;

		// Identify Piece Type and Color using bitmasks
		if (state.whitePawns & fromMask) { pt = PieceType::Pawn; color = Color::White; }
		else if (state.blackPawns & fromMask) { pt = PieceType::Pawn; color = Color::Black; }
		else if (state.whiteKings & fromMask) { pt = PieceType::King; color = Color::White; }
		else if (state.blackKings & fromMask) { pt = PieceType::King; color = Color::Black; }

		if (pt == PieceType::None) return;

		// 2. Generate Moves for Sliding Pieces, Knights, and King
		if (pt != PieceType::Pawn) {



			// Castling Logic (King Only)
			if (pt == PieceType::King && !this->isCheck()) {
				int piecePos = (color == Color::White) ? 7 : 0; // Visual rank index for King
				// Note: Position uses row 0-7. In standard Bitboard mapping used here:
				// White moves +8, Black moves -8. White King starts at bit-index 60 (row 0 in logic).
				// We keep the original Position-based logic for rows.

				bool kingSide = (color == Color::White) ? castling.whiteKing : castling.blackKing;
				bool queenSide = (color == Color::White) ? castling.whiteQueen : castling.blackQueen;

				if (queenSide) {
					if (bitboard.canCastle(false, fromPos)) {
						moves.push_back(Move(fromPos.row, fromPos.column, piecePos, 2, false, true));
					}
				}
				if (kingSide) {
					if (bitboard.canCastle(true, fromPos)) {
						moves.push_back(Move(fromPos.row, fromPos.column, piecePos, 6, false, true));
					}
				}
			}
		}
		// 3. Generate Pawn Moves using Bitwise Logic
		else {
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
				Position toPos = chess::Position::numToPosition(targetSq);

				if (targetRank == promoRankIdx) {
					moves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column, false, false, chess::PieceType::Queen));
					moves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column, false, false, chess::PieceType::Rook));
					moves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column, false, false, chess::PieceType::Bishop));
					moves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column, false, false, chess::PieceType::Knight));
				}
				else {
					moves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column));

					// --- Double Push ---
					// Only possible if Single Push was valid and we are on start rank
					if ((7 - (sq / 8)) == startRankIdx) {
						int doubleTarget = sq + (forward * 2);
						if (!(allPieces & (1ULL << doubleTarget))) {
							Position toPosDouble = chess::Position::numToPosition(doubleTarget);
							moves.push_back(Move(fromPos.row, fromPos.column, toPosDouble.row, toPosDouble.column));
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
					Position toPos = chess::Position::numToPosition(capSq);

					if (targetRank == promoRankIdx) {
						moves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column, false, false, chess::PieceType::Queen));
						moves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column, false, false, chess::PieceType::Rook));
						moves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column, false, false, chess::PieceType::Bishop));
						moves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column, false, false, chess::PieceType::Knight));
					}
					else {
						moves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column));
					}
				}
			}

			// --- En Passant ---
			// Requires checking History for the last move
			if (!history.empty()) {
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
						int lastTo = lastMove.getToPos().getNumberIndex();
						int lastFrom = lastMove.getFromPos().getNumberIndex();

						if (lastTo == adjSq && abs(lastFrom - lastTo) == 16) {
							// Valid En Passant Target
							int destSq = adjSq + forward; // Move to the square *behind* the pawn
							Position toPos = chess::Position::numToPosition(destSq);

							auto enPassantMove = Move(fromPos.row, fromPos.column, toPos.row, toPos.column, true);

							// Original logic used !canEnPassant to validate safety
							if (!bitboard.canEnPassant(enPassantMove)) {
								moves.push_back(enPassantMove);
							}
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
			auto fromPos = chess::Position::numToPosition(fromSq);
			auto knightMoves = bitboard.getAllKnightMoves(fromSq,sideToMove);
			while (knightMoves) {
				int toSq = Bitboard::lsb(knightMoves);   // index of least-significant 1-bit
				knightMoves &= knightMoves - 1;
				auto toPos = chess::Position::numToPosition(toSq);
				allMoves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column));
			}
		}
		while (myRooks) {
			int fromSq = Bitboard::lsb(myRooks);   // index of least-significant 1-bit
			myRooks &= myRooks - 1;       // remove that bit
			auto fromPos = chess::Position::numToPosition(fromSq);
			auto rookMoves = bitboard.getAllRookMoves(fromSq, sideToMove);
			while (rookMoves) {
				int toSq = Bitboard::lsb(rookMoves);   // index of least-significant 1-bit
				rookMoves &= rookMoves - 1;
				auto toPos = chess::Position::numToPosition(toSq);
				allMoves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column));
			}
		}
		while (myBishops) {
			int fromSq = Bitboard::lsb(myBishops);   // index of least-significant 1-bit
			myBishops &= myBishops - 1;       // remove that bit
			auto fromPos = chess::Position::numToPosition(fromSq);
			auto bishopMoves = bitboard.getAllBishopMoves(fromSq, sideToMove);
			while (bishopMoves) {
				int toSq = Bitboard::lsb(bishopMoves);   // index of least-significant 1-bit
				bishopMoves &= bishopMoves - 1;
				auto toPos = chess::Position::numToPosition(toSq);
				allMoves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column));
			}
		}
		while (myQueens) {
			int fromSq = Bitboard::lsb(myQueens);   // index of least-significant 1-bit
			myQueens &= myQueens - 1;       // remove that bit
			auto fromPos = chess::Position::numToPosition(fromSq);
			auto queensMoves = bitboard.getAllQueenMoves(fromSq, sideToMove);
			while (queensMoves) {
				int toSq = Bitboard::lsb(queensMoves);   // index of least-significant 1-bit
				queensMoves &= queensMoves - 1;
				auto toPos = chess::Position::numToPosition(toSq);
				allMoves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column));
			}
		}
		while (myKings) {
			int fromSq = Bitboard::lsb(myKings);   // index of least-significant 1-bit
			myKings &= myKings - 1;       // remove that bit
			auto fromPos = chess::Position::numToPosition(fromSq);
			auto kingMoves = bitboard.getAllKingMoves(fromSq, sideToMove);
			while (kingMoves) {
				int toSq = Bitboard::lsb(kingMoves);   // index of least-significant 1-bit
				kingMoves &= kingMoves - 1;
				auto toPos = chess::Position::numToPosition(toSq);
				allMoves.push_back(Move(fromPos.row, fromPos.column, toPos.row, toPos.column));
			}
		}
		uint64_t pieces = (myPawns | myKings);
		while (pieces) {
			// Get the index of the first piece (0-63)
			int sq = Bitboard::lsb(pieces);
			auto pos = chess::Position(sq);

			generatePieceMoves(pos, allMoves);

			pieces &= (pieces - 1);

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
				pseudoMoves = MoveList();
				generatePieceMoves(chess::Position::numToPosition(kingIndex), pseudoMoves);
			}
		}
		auto pinned = bitboard.getPinnedPieces();
		IndexSet pinnedPieces = pinned.first;
		auto pinMask = pinned.second;

		for (Move mv : pseudoMoves) {
			Position fromPos = mv.getFromPos();
			Position toPos = mv.getToPos();
			int fromPosNum = chess::Position::positionToNum(fromPos);
			int toPosNum = chess::Position::positionToNum(toPos);
			auto k = sideToMove;

			if (this->isCheck()) {
				std::pair<uint64_t, uint64_t> mask = bitboard.getCheckMask(kingIndex, sideToMove, attackers);
				uint64_t checkMask = mask.first;
				uint64_t controledMask = mask.second;
				if (bitboard.getPieceType(fromPos)==chess::PieceType::King) {

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
				else if (bitboard.getPieceType(fromPos) == chess::PieceType::King) {
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
		return bitboard.isCheck(sideToMove);
	}
	bool Board::isCheckMate() {
		auto moves = getAllLegalMoves();
		if (moves.count == 0) {
			return isCheck();
		}
		return false;
	}
}