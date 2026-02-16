#include "Board.h"
#include <cmath>
#include <algorithm>
namespace chess {
	static char pieceToSANLetter(chess::PieceType pt) {
		switch (pt) {
		case chess::PieceType::King:   return 'K';
		case chess::PieceType::Queen:  return 'Q';
		case chess::PieceType::Rook:   return 'R';
		case chess::PieceType::Bishop: return 'B';
		case chess::PieceType::Knight: return 'N';
		default: return '\0'; // pawn has no letter
		}
	}

	static char promoToLower(chess::PieceType pt) {
		switch (pt) {
		case chess::PieceType::Queen:  return 'Q';
		case chess::PieceType::Rook:   return 'R';
		case chess::PieceType::Bishop: return 'B';
		case chess::PieceType::Knight: return 'N';
		default: return '\0';
		}
	}
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
					moves.push_back(Move(sq, capSq, false, Castling::NONE, chess::PieceType::Queen), true);
					moves.push_back(Move(sq, capSq, false, Castling::NONE, chess::PieceType::Rook), true);
					moves.push_back(Move(sq, capSq, false, Castling::NONE, chess::PieceType::Bishop), true);
					moves.push_back(Move(sq, capSq, false, Castling::NONE, chess::PieceType::Knight), true);
				}
				else {
					moves.push_back(Move(sq, capSq), true);
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
					int lastTo = lastMove.getToSq();
					int lastFrom = lastMove.getFromSq();

					if (lastTo == adjSq && abs(lastFrom - lastTo) == 16) {
						// Valid En Passant Target
						int destSq = adjSq + forward; // Move to the square *behind* the pawn

						auto enPassantMove = Move(sq, destSq, true);

						// Original logic used !canEnPassant to validate safety
						if (!bitboard.canEnPassant(enPassantMove)) {
							moves.push_back(enPassantMove, true);
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
			if (!isCheck()) {

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

		allMoves.finalize();
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
				if (bitboard.getPieceType(fromPosNum) == chess::PieceType::King) {

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
						legalMoves.push_back(mv, bitboard.hasPiece(mv.getToSq()));
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
					legalMoves.push_back(mv, bitboard.hasPiece(mv.getToSq()));

				}
			}

		}
		legalMoves.finalize();
		return legalMoves;

	}
	std::string Board::moveToSAN(const chess::Move& mv)
	{
		// --- 0) Castling ---
		if (mv.isCastling()) {
			std::string s =
				(mv.getFlags() == chess::QUEEN_CASTLE) ? "O-O-O" : "O-O";

			Board copy = *this;
			copy.makeMove(mv);
			copy.check = Check::Undefined; // reset cache

			if (copy.isCheckMate()) s += "#";
			else if (copy.isCheck()) s += "+";

			return s;
		}

		std::string san;

		chess::Square fromSq = getSquare(mv.getFromPos());
		chess::Piece movedPiece = fromSq.getPiece();
		chess::PieceType pt = movedPiece.getPieceType();

		chess::Position from = mv.getFromPos();
		chess::Position to = mv.getToPos();

		// --- 1) Detect capture correctly ---
		bool isCapture = false;

		if (mv.isEnPassant())
			isCapture = true;
		else if (getSquare(to).hasPiece())
			isCapture = true;

		// --- 2) Piece letter (no letter for pawn) ---
		char pieceLetter = '\0';
		switch (pt) {
		case chess::PieceType::King:   pieceLetter = 'K'; break;
		case chess::PieceType::Queen:  pieceLetter = 'Q'; break;
		case chess::PieceType::Rook:   pieceLetter = 'R'; break;
		case chess::PieceType::Bishop: pieceLetter = 'B'; break;
		case chess::PieceType::Knight: pieceLetter = 'N'; break;
		default: break;
		}

		if (pieceLetter != '\0')
			san += pieceLetter;

		// --- 3) Disambiguation (correct logic) ---
		if (pieceLetter != '\0') {

			MoveList allMoves = getAllLegalMoves();

			bool sameFileConflict = false;
			bool sameRankConflict = false;
			bool conflictExists = false;

			for (int i = 0; i < allMoves.count; ++i) {
				const Move& other = allMoves.moves[i];

				if (other == mv)
					continue;

				if (other.getToSq() != mv.getToSq())
					continue;

				chess::Square s2 = getSquare(other.getFromPos());
				if (!s2.hasPiece())
					continue;

				if (s2.getPiece().getPieceType() != pt)
					continue;

				if (s2.getPiece().getColor() != movedPiece.getColor())
					continue;

				conflictExists = true;

				chess::Position otherFrom = other.getFromPos();

				if (otherFrom.column == from.column)
					sameFileConflict = true;

				if (otherFrom.row == from.row)
					sameRankConflict = true;
			}

			if (conflictExists) {
				if (!sameFileConflict)
					san += char('a' + from.column);
				else if (!sameRankConflict)
					san += char('1' + (7 - from.row));
				else {
					san += char('a' + from.column);
					san += char('1' + (7 - from.row));
				}
			}
		}

		// --- 4) Pawn capture must include file ---
		if (pt == chess::PieceType::Pawn && isCapture) {
			san += char('a' + from.column);
		}

		// --- 5) Capture marker ---
		if (isCapture)
			san += "x";

		// --- 6) Destination square ---
		san += chess::Position::squareToString(to);

		// --- 7) Promotion ---
		chess::PieceType prom = mv.getPromotionPiece();
		if (prom != chess::PieceType::None) {
			san += "=";
			switch (prom) {
			case chess::PieceType::Queen:  san += "Q"; break;
			case chess::PieceType::Rook:   san += "R"; break;
			case chess::PieceType::Bishop: san += "B"; break;
			case chess::PieceType::Knight: san += "N"; break;
			default: break;
			}
		}

		// --- 8) Check / Checkmate ---
		Board copy = *this;
		copy.makeMove(mv);
		copy.check = Check::Undefined; // IMPORTANT

		if (copy.isCheckMate())
			san += "#";
		else if (copy.isCheck())
			san += "+";

		return san;
	}
	MoveList Board::getAllLegalCaptures() {
		MoveList captureMoves = getAllLegalMoves();
		captureMoves.finalizeCapturesOnly();
		return captureMoves;
	}
	static double totalNodesGeometric(double b, int depth) {
		if (depth < 0) return 0.0;
		if (std::abs(b - 1.0) < 1e-9) return double(depth + 1); // 1 + 1 + ... + 1
		return (std::pow(b, depth + 1) - 1.0) / (b - 1.0);
	}

	double Board::estimatePositions(int depth) {
		int m = getAllLegalMoves().count;
		// clamp so we don't get silly values in crazy positions
		double md = std::max(1, m);

		// choose a shrink factor by depth
		double k = (depth <= 7) ? 0.75 : 0.70;   // for depth 9 use 0.70
		double b_eff = std::max(1.0, k * md);


		return totalNodesGeometric(b_eff, depth);
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