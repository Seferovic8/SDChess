#include "Board.h"

namespace chess {
	MoveList Board::generatePieceMoves(chess::Position fromPos) {
		Piece piece = board[fromPos.row][fromPos.column].getPiece();
		Color pieceColor = piece.getColor();
		MoveList moves;
		auto dirs = piece.directions();
		for (chess::Position direction : dirs) {
			if (piece.isSliding()) {
				int row = fromPos.row;
				int col = fromPos.column;
				row += direction.row;
				col += direction.column;
				while (isInside(row, col)) {
					if (board[row][col].hasPiece()) {
						if (piece.getColor() != board[row][col].getPiece().getColor()) {
							moves.push_back(Move(fromPos.row, fromPos.column, row, col));
						}
						break;
					}
					else {

						moves.push_back(Move(fromPos.row, fromPos.column, row, col));
					}
					row += direction.row;
					col += direction.column;

				}
			}
			else {
				int row = fromPos.row + direction.row;
				int col = fromPos.column + direction.column;
				if (isInside(row, col)) {
					if (board[row][col].hasPiece()) {
						if (piece.getColor() != board[row][col].getPiece().getColor()) {
							moves.push_back(Move(fromPos.row, fromPos.column, row, col));
						}
					}
					else {
						moves.push_back(Move(fromPos.row, fromPos.column, row, col));
					}
				}

			}
		}
		if (!this->isCheck()) {

			if (piece.getPieceType() == PieceType::King) {
				int row = fromPos.row;
				int col = fromPos.column;
				int piecePos = (piece.getColor() == Color::White) ? 7 : 0;
				bool& kingRight = (piece.getColor() == Color::White) ? castling.whiteKing : castling.blackKing;
				bool& queenRight = (piece.getColor() == Color::White) ? castling.whiteQueen : castling.blackQueen;
				if (queenRight) {


					if (bitboard.canCastle(false, fromPos))
					{
						moves.push_back(Move(fromPos.row, fromPos.column, piecePos, 2, false, true));
					}

				}

				if (kingRight) {
					if (bitboard.canCastle(true, fromPos))
					{
						moves.push_back(Move(fromPos.row, fromPos.column, piecePos, 6, false, true));
					}

				}
			}

		}
		if (piece.getPieceType() == PieceType::Pawn) {
			int row = fromPos.row;
			int col = fromPos.column;
			int dir = (piece.getColor() == Color::White) ? -1 : 1;   // move direction
			int startRow = (piece.getColor() == Color::White) ? 6 : 1;
			int enPassantRow = (piece.getColor() == Color::White) ? 3 : 4;
			int promotionRow = (piece.getColor() == Color::White) ? 0 : 7;

			// 1️⃣ Single push
			int r1 = row + dir;
			if (isInside(r1, col) && !board[r1][col].hasPiece()) {

				if (r1 != promotionRow) {

					moves.push_back(Move(row, col, r1, col));
				}
				else {
					moves.push_back(Move(row, col, r1, col, false, false, chess::PieceType::Queen));
					moves.push_back(Move(row, col, r1, col, false, false, chess::PieceType::Rook));
					moves.push_back(Move(row, col, r1, col, false, false, chess::PieceType::Bishop));
					moves.push_back(Move(row, col, r1, col, false, false, chess::PieceType::Knight));

				}
				// 2️⃣ Double push from start
				int r2 = row + 2 * dir;
				if (row == startRow && !board[r2][col].hasPiece()) {
					moves.push_back(Move(row, col, r2, col));
				}
			}

			// 3️⃣ Diagonal captures (±1 col)
			const int dc[2] = { -1, 1 };
			for (int d = 0; d < 2; d++) {
				int cc = col + dc[d];
				int rr = row + dir;

				if (isInside(rr, cc) && board[rr][cc].hasPiece()) {
					if (board[rr][cc].getPiece().getColor() != piece.getColor()) {
						if (rr != promotionRow) {
							moves.push_back(Move(row, col, rr, cc));
						}
						else {
							moves.push_back(Move(row, col, rr, cc, false, false, chess::PieceType::Queen));
							moves.push_back(Move(row, col, rr, cc, false, false, chess::PieceType::Rook));
							moves.push_back(Move(row, col, rr, cc, false, false, chess::PieceType::Bishop));
							moves.push_back(Move(row, col, rr, cc, false, false, chess::PieceType::Knight));
						}
					}
				}
			}
			// 4️⃣ En passant
			if (row == enPassantRow && !history.empty()) {

				auto lastMove = history.back().move;
				for (int d = 0; d < 2; d++) {
					int cc = col + dc[d];
					if (!isInside(row, cc)) continue;

					if (!board[row][cc].hasPiece()) continue;

					if (board[row][cc].getPiece().getPieceType() != chess::PieceType::Pawn) continue;

					auto target = board[row][cc].getPiece();
					if (!(piece.getPieceType() == PieceType::Pawn)) continue;

					if (lastMove.getFromPos().row != 1 && lastMove.getFromPos().row != 6) continue;

					// the last move ended on that square
					if (lastMove.getToPos() == chess::Position(row, cc)) {

						auto enPassantMove = Move(row, col, row + dir, cc, true);
						if (!bitboard.canEnPassant(enPassantMove)) {
							moves.push_back(enPassantMove);

						}
					}
				}
			}
		}

		return moves;
	}
	MoveList Board::getAllPseudoLegalMoves() {
		MoveList allMoves;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (!board[i][j].hasPiece()) continue;
				auto pieceColor = board[i][j].getPiece().getColor();
				if (pieceColor == sideToMove) {
					auto pieceMoves = generatePieceMoves(chess::Position(i, j));
					allMoves.insert(allMoves.end(), pieceMoves.begin(), pieceMoves.end());
				}
			}

		}
		return allMoves;
	}
	MoveList Board::getAllLegalMoves() {
		MoveList legalMoves;
		bitboard.loadBitboard(board, sideToMove);
		uint64_t controled = bitboard.controledSquares(!sideToMove);
		int kingIndex = bitboard.getKingIndex(sideToMove);

		uint64_t attackers = bitboard.attackersToKing(sideToMove);
		MoveList pseudoMoves = getAllPseudoLegalMoves();
		if (this->isCheck()) {
			int numAttackers = bitboard.getBitList(attackers).size();
			if (numAttackers >= 2) {
				pseudoMoves = generatePieceMoves(chess::Position::numToPosition(kingIndex));
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
				if (board[fromPos.row][fromPos.column].isKing()) {

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
				else if (board[fromPos.row][fromPos.column].isKing()) {
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