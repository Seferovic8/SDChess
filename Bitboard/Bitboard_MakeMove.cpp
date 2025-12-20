#include "Bitboard.h"
namespace chess {
	StateBitboard Bitboard::getBitboardState() {
		return StateBitboard(whitePawns, whiteQueens, whiteKings, whiteBishops, whiteKnights, whiteRooks,
			blackPawns, blackQueens, blackKings, blackBishops, blackKnights, blackRooks);
	}
	void Bitboard::loadBitboardFromState(StateBitboard state, Color side) {
		whitePawns = state.whitePawns;
		whiteQueens = state.whiteQueens;
		whiteKings = state.whiteKings;
		whiteBishops = state.whiteBishops;
		whiteKnights = state.whiteKnights;
		whiteRooks = state.whiteRooks;

		blackPawns = state.blackPawns;
		blackQueens = state.blackQueens;
		blackKings = state.blackKings;
		blackBishops = state.blackBishops;
		blackKnights = state.blackKnights;
		blackRooks = state.blackRooks;

		updateOccupancy();
		sideToMove=side;
	}
	void Bitboard::removeEnemyPiece(Color enemy, chess::Position pos) {
		uint64_t mask = ~(1ULL << pos.getNumberIndex());

		if (enemy == Color::White) {
			whitePawns &= mask;
			whiteKnights &= mask;
			whiteBishops &= mask;
			whiteRooks &= mask;
			whiteQueens &= mask;
			whiteKings &= mask;
		}
		else {
			blackPawns &= mask;
			blackKnights &= mask;
			blackBishops &= mask;
			blackRooks &= mask;
			blackQueens &= mask;
			blackKings &= mask;
		}
	}
	void Bitboard::movePieceOnBitboard(int fromSq, int toSq, PieceType pt, Color side)
	{
		if (pt == chess::PieceType::None) return;
		uint64_t fromMask = (1ULL << fromSq);
		uint64_t toMask = (1ULL << toSq);

		uint64_t* bb = nullptr;

		if (side == Color::White) {
			switch (pt) {
			case chess::PieceType::Pawn:   bb = &whitePawns; break;
			case chess::PieceType::Knight: bb = &whiteKnights; break;
			case chess::PieceType::Bishop: bb = &whiteBishops; break;
			case chess::PieceType::Rook:   bb = &whiteRooks; break;
			case chess::PieceType::Queen:  bb = &whiteQueens; break;
			case chess::PieceType::King:   bb = &whiteKings; break;
			}
		}
		else {
			switch (pt) {
			case chess::PieceType::Pawn:   bb = &blackPawns; break;
			case chess::PieceType::Knight: bb = &blackKnights; break;
			case chess::PieceType::Bishop: bb = &blackBishops; break;
			case chess::PieceType::Rook:   bb = &blackRooks; break;
			case chess::PieceType::Queen:  bb = &blackQueens; break;
			case chess::PieceType::King:   bb = &blackKings; break;
			}
		}

		// REMOVE FROM SOURCE
		*bb &= ~fromMask;

		// ADD TO DESTINATION
		*bb |= toMask;
	}
	void Bitboard::handlePromotion(Color side, int toSq, PieceType promo)
	{
		uint64_t mask = (1ULL << toSq);

		// 1. Remove pawn from destination square
		if (side == Color::White) {
			whitePawns &= ~mask;

			switch (promo) {
			case chess::PieceType::Queen:  whiteQueens |= mask; break;
			case chess::PieceType::Rook:   whiteRooks |= mask; break;
			case chess::PieceType::Bishop: whiteBishops |= mask; break;
			case chess::PieceType::Knight: whiteKnights |= mask; break;
			default: break;
			}
		}
		else {
			blackPawns &= ~mask;

			switch (promo) {
			case chess::PieceType::Queen:  blackQueens |= mask; break;
			case chess::PieceType::Rook:   blackRooks |= mask; break;
			case chess::PieceType::Bishop: blackBishops |= mask; break;
			case chess::PieceType::Knight: blackKnights |= mask; break;
			default: break;
			}
		}
		//updateOccupancy();

	}
	void Bitboard::handleEnPassant(const Move& m, Color side)
	{
		int fromSq = m.getFromPos().getNumberIndex();
		int toSq = m.getToPos().getNumberIndex();
		movePieceOnBitboard(fromSq, toSq, chess::PieceType::Pawn, side);

		// Captured pawn is behind the destination square
		int capturedSq;

		if (side == Color::White) {
			// White EP: pawn moves upward → captured pawn is one rank below
			capturedSq = toSq - 8;
		}
		else {
			// Black EP: pawn moves downward → captured pawn is one rank above
			capturedSq = toSq + 8;
		}

		uint64_t mask = ~(1ULL << capturedSq);

		if (side == Color::White) {
			// black pawn is captured
			blackPawns &= mask;
		}
		else {
			// white pawn is captured
			whitePawns &= mask;
		}
		updateOccupancy();

	}

	void Bitboard::makeMove(const Move& m, PieceType pt, Color pieceColor, bool isCapture)
	{
		int fromSq = m.getFromPos().getNumberIndex();
		int toSq = m.getToPos().getNumberIndex();

		// 1. Remove captured piece
		if (isCapture) {
			removeEnemyPiece(!pieceColor, toSq);
		}
		movePieceOnBitboard(fromSq, toSq, pt, pieceColor);

		// 3. Promotion
		if (m.getPromotionPiece() != PieceType::None) {
			handlePromotion(pieceColor, toSq, m.getPromotionPiece());
		}

		//// 4. Castling
		//if (m.isCastling()) {
		//	handleCastling(m,side);
		//}

		//// 5. En passant
		//if (m.isEnPassant()) {
		//	handleEnPassant(side, fromSq,toSq);
		//}

		// 6. Recompute occupancy
		updateOccupancy();
	}

	void Bitboard::handleCastling(const Move& m, Color side)
	{

		// You know the king moves from e-file to c or g.
		int kingFrom = m.getFromPos().getNumberIndex();
		int kingTo = m.getToPos().getNumberIndex();
		movePieceOnBitboard(kingFrom, kingTo, chess::PieceType::King, side);

		// Determine rook movement
		int rookFrom, rookTo;

		bool isKingSide = (kingTo > kingFrom); // g > e

		if (side == Color::White) {
			if (isKingSide) {
				rookFrom = 7;  // h1
				rookTo = 5;  // f1
			}
			else {
				rookFrom = 0;  // a1
				rookTo = 3;  // d1
			}
		}
		else { // black
			if (isKingSide) {
				rookFrom = 63; // h8
				rookTo = 61; // f8
			}
			else {
				rookFrom = 56; // a8
				rookTo = 59; // d8
			}
		}

		uint64_t rookMaskFrom = (1ULL << rookFrom);
		uint64_t rookMaskTo = (1ULL << rookTo);

		// -------------------------------
		// Move rook in its bitboard
		// -------------------------------
		if (side == Color::White) {
			whiteRooks &= ~rookMaskFrom;
			whiteRooks |= rookMaskTo;
		}
		else {
			blackRooks &= ~rookMaskFrom;
			blackRooks |= rookMaskTo;
		}
		updateOccupancy();
	}
	void Bitboard::updateOccupancy() {
		whitePieces =
			whitePawns | whiteKnights | whiteBishops |
			whiteRooks | whiteQueens | whiteKings;

		blackPieces =
			blackPawns | blackKnights | blackBishops |
			blackRooks | blackQueens | blackKings;

		allPieces = whitePieces | blackPieces;
		sideToMove = !sideToMove;
	}

	void Bitboard::unmakeMove(const Move& m, PieceType movedPiece, PieceType capturedPiece, Color sideThatMoved) {
		int fromSq = m.getFromPos().getNumberIndex();
		int toSq = m.getToPos().getNumberIndex();
		uint64_t fromMask = (1ULL << fromSq);
		uint64_t toMask = (1ULL << toSq);

		// ---------------------------------------------------------
		// 1. Handle En Passant
		// ---------------------------------------------------------
		// En Passant is unique because the captured piece is NOT at 'toSq'
		if (m.isEnPassant()) {
			// Move the pawn back: toSq -> fromSq
			movePieceOnBitboard(toSq, fromSq, PieceType::Pawn, sideThatMoved);

			// Restore the captured pawn to the correct square
			// White EP: moves up, captured pawn was below (toSq - 8)
			// Black EP: moves down, captured pawn was above (toSq + 8)
			int capSq = (sideThatMoved == Color::White) ? (toSq - 8) : (toSq + 8);
			uint64_t capMask = (1ULL << capSq);

			if (sideThatMoved == Color::White) {
				blackPawns |= capMask; // Restore Black Pawn
			}
			else {
				whitePawns |= capMask; // Restore White Pawn
			}
		}
		// ---------------------------------------------------------
		// 2. Handle Castling
		// ---------------------------------------------------------
		else if (m.isCastling()) {
			// Move King back: toSq -> fromSq
			movePieceOnBitboard(toSq, fromSq, PieceType::King, sideThatMoved);

			// Determine Rook positions (logic inverted from makeMove)
			bool isKingSide = (toSq > fromSq);
			int rookFrom, rookTo;

			if (sideThatMoved == Color::White) {
				rookFrom = isKingSide ? 7 : 0; // h1 or a1
				rookTo = isKingSide ? 5 : 3;   // f1 or d1
			}
			else {
				rookFrom = isKingSide ? 63 : 56; // h8 or a8
				rookTo = isKingSide ? 61 : 59;   // f8 or d8
			}

			// Move Rook back: from 'rookTo' (current pos) to 'rookFrom' (original pos)
			movePieceOnBitboard(rookTo, rookFrom, PieceType::Rook, sideThatMoved);
		}
		// ---------------------------------------------------------
		// 3. Handle Promotion
		// ---------------------------------------------------------
		else if (m.getPromotionPiece() != PieceType::None) {
			// The piece currently at 'toSq' is the PROMOTED piece (e.g., Queen).
			// We must remove it manually.
			uint64_t* promoBB = nullptr;

			if (sideThatMoved == Color::White) {
				switch (m.getPromotionPiece()) {
				case PieceType::Queen:  promoBB = &whiteQueens; break;
				case PieceType::Rook:   promoBB = &whiteRooks; break;
				case PieceType::Bishop: promoBB = &whiteBishops; break;
				case PieceType::Knight: promoBB = &whiteKnights; break;
				default: break;
				}
				if (promoBB) *promoBB &= ~toMask; // Remove promoted piece
				whitePawns |= fromMask;           // Place original Pawn back at fromSq
			}
			else {
				switch (m.getPromotionPiece()) {
				case PieceType::Queen:  promoBB = &blackQueens; break;
				case PieceType::Rook:   promoBB = &blackRooks; break;
				case PieceType::Bishop: promoBB = &blackBishops; break;
				case PieceType::Knight: promoBB = &blackKnights; break;
				default: break;
				}
				if (promoBB) *promoBB &= ~toMask; // Remove promoted piece
				blackPawns |= fromMask;           // Place original Pawn back at fromSq
			}
		}
		// ---------------------------------------------------------
		// 4. Standard Move
		// ---------------------------------------------------------
		else {
			// Simply move the piece back: toSq -> fromSq
			movePieceOnBitboard(toSq, fromSq, movedPiece, sideThatMoved);
		}

		// ---------------------------------------------------------
		// 5. Restore Captured Piece (Standard & Promotion captures)
		// ---------------------------------------------------------
		// Note: We skip this for En Passant as it was handled in step 1.
		if (capturedPiece != PieceType::None && !m.isEnPassant()) {
			Color enemy = !sideThatMoved;
			uint64_t* captureBB = nullptr;

			// Determine which bitboard to restore the piece to
			if (enemy == Color::White) {
				switch (capturedPiece) {
				case PieceType::Pawn:   captureBB = &whitePawns; break;
				case PieceType::Knight: captureBB = &whiteKnights; break;
				case PieceType::Bishop: captureBB = &whiteBishops; break;
				case PieceType::Rook:   captureBB = &whiteRooks; break;
				case PieceType::Queen:  captureBB = &whiteQueens; break;
				case PieceType::King:   captureBB = &whiteKings; break;
				default: break;
				}
			}
			else {
				switch (capturedPiece) {
				case PieceType::Pawn:   captureBB = &blackPawns; break;
				case PieceType::Knight: captureBB = &blackKnights; break;
				case PieceType::Bishop: captureBB = &blackBishops; break;
				case PieceType::Rook:   captureBB = &blackRooks; break;
				case PieceType::Queen:  captureBB = &blackQueens; break;
				case PieceType::King:   captureBB = &blackKings; break;
				default: break;
				}
			}

			// Add the captured piece back to 'toSq'
			if (captureBB) *captureBB |= toMask;
		}

		// 6. Update global bitboards (occupancy)
		updateOccupancy();

		// Force sideToMove back to the side that moved (because updateOccupancy flips it)
		sideToMove = sideThatMoved;
	}
}