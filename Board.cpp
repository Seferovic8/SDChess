#include<iostream>
#include <unordered_set>
#include <array>   

#include <string>
#include <vector>
#include "Board.h"
#include "Move.h"
#include "Square.h"
namespace chess {

	Board::Board() {
		board.resize(8);
		for (int r = 0; r < 8; r++) {
			board[r].resize(8);

			for (int c = 0; c < 8; c++) {
				if (r == 0 || r == 1 || r == 6 || r == 7) {
					if (r == 1) {
						board[r][c] = Square(r, c, Piece(Color::White,chess::PieceType::Pawn));
					}
					else if (r == 6) {
						board[r][c] = Square(r, c, Piece(Color::Black, chess::PieceType::Pawn));
					}
					else {

						switch (c) {
						case 0: {

							if (r == 0) {
								board[r][c] = Square(r, c, Piece(Color::White, chess::PieceType::Rook));
							}
							else {
								board[r][c] = Square(r, c, Piece(Color::Black, chess::PieceType::Rook));
							}
							break;
						}
						case 1: {

							if (r == 0) {
								board[r][c] = Square(r, c, Piece(Color::White, chess::PieceType::Knight));
							}
							else {
								board[r][c] = Square(r, c, Piece(Color::Black, chess::PieceType::Knight));
							}
							break;
						}
						case 2: {

							if (r == 0) {
								board[r][c] = Square(r, c, Piece(Color::White, chess::PieceType::Bishop));
							}
							else {
								board[r][c] = Square(r, c, Piece(Color::Black, chess::PieceType::Bishop));
							}
							break;
						}
						case 3: {

							if (r == 0) {
								board[r][c] = Square(r, c, Piece(Color::White, chess::PieceType::Queen));
							}
							else {
								board[r][c] = Square(r, c, Piece(Color::Black, chess::PieceType::Queen));
							}
							break;
						}
						case 4: {

							if (r == 0) {
								board[r][c] = Square(r, c, Piece(Color::White, chess::PieceType::King));
							}
							else {
								board[r][c] = Square(r, c, Piece(Color::Black, chess::PieceType::King));
							}
							break;
						}
						case 5: {

							if (r == 0) {
								board[r][c] = Square(r, c, Piece(Color::White, chess::PieceType::Bishop));
							}
							else {
								board[r][c] = Square(r, c, Piece(Color::Black, chess::PieceType::Bishop));
							}
							break;
						}
						case 6: {

							if (r == 0) {
								board[r][c] = Square(r, c, Piece(Color::White, chess::PieceType::Knight));
							}
							else {
								board[r][c] = Square(r, c, Piece(Color::Black, chess::PieceType::Knight));
							}
							break;
						}
						case 7: {
							if (r == 0) {
								board[r][c] = Square(r, c, Piece(Color::White, chess::PieceType::Rook));
							}
							else {
								board[r][c] = Square(r, c, Piece(Color::Black, chess::PieceType::Rook));
							}
							break;
						}
						}
					}
				}
				else {

					board[r][c] = Square(r, c);
				}
			}
		}
		sideToMove = Color::White;
		bitboard.loadBitboard(board, sideToMove);
	}
	Board::Board(std::string fen) {
		//history.push_back(Move("B7", "B5"));

		board.resize(8);
		for (int r = 0; r < 8; r++) {
			board[r].resize(8);
		}
		int row = 0, col = 0;

		int i = 0;

		// --- Parse board ---
		for (; i < fen.size(); i++) {
			char c = fen[i];

			if (c == ' ') {
				break;
			} // stop at end of board section

			if (c == '/') {
				row++;
				col = 0;
			}
			else if (isdigit(c)) {
				board[row][col] = Square(row, col);
				col += c - '0';  // skip empty squares

			}
			else {

				board[row][col] = Square(row, col, chess::createPieceFromChar(c));
				col++;
			}
		}
		if (fen[i + 1] == 'w')
			sideToMove = Color::White;
		else if (fen[i + 1] == 'b')
			sideToMove = Color::Black;
		else
			throw std::runtime_error("Invalid FEN: expecting w or b");

		int b = i + 3;
		CastlingRights cast;
		cast.blackKing = false;
		cast.blackQueen = false;
		cast.whiteKing = false;
		cast.whiteQueen = false;
		while (fen[b] != ' ') {
			switch (fen[b]) {
			case 'K':
				cast.whiteKing = true;
				break;
			case 'Q':
				cast.whiteQueen = true;
				break;
			case 'k':
				cast.blackKing = true;
				break;
			case 'q':
				cast.blackQueen = true;
				break;
			}
			b++;
		}
		castling = cast;
		bitboard.loadBitboard(board, sideToMove);

	}
	void Board::printBoard() const {
		for (int r = 0; r < 8; r++) {
			for (int c = 0; c < 8; c++) {
				std::cout << board[r][c].getLabel() << ' ';
				//return;
			}
			std::cout << std::endl;
		}
	}
	bool Board::isInside(int r, int c) {
		return !(r > 7 || r < 0 || c>7 || c < 0);
	}
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

					if (board[row][cc].getPiece().getPieceType()!=chess::PieceType::Pawn) continue;

					auto target = board[row][cc].getPiece();
					if (!(piece.getPieceType() == PieceType::Pawn)) continue;

					if (lastMove.getFromPos().row!=1 && lastMove.getFromPos().row != 6) continue;

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
				pseudoMoves = generatePieceMoves(chess::Bitboard::numToPosition(kingIndex));
			}
		}
		auto pinned = bitboard.getPinnedPieces();
		IndexSet pinnedPieces = pinned.first;
		auto pinMask = pinned.second;

		for (Move mv : pseudoMoves) {
			Position fromPos = mv.getFromPos();
			Position toPos = mv.getToPos();
			int fromPosNum = chess::Bitboard::positionToNum(fromPos);
			int toPosNum = chess::Bitboard::positionToNum(toPos);
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
	void Board::rookCastling(chess::Position pos,Color pieceColor) {
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
	void Board::makeMove(chess::Move move) {
		chess::Position fromPos = move.getFromPos();
		chess::Position toPos = move.getToPos();



		chess::PieceType pieceType = board[fromPos.row][fromPos.column].getPiece().getPieceType();
		chess::Color pieceColor = board[fromPos.row][fromPos.column].getPiece().getColor();
		chess::CastlingRights previousCastling = castling;
		auto k = move.getMoveText();
		sideToMove = !sideToMove;
		board[fromPos.row][fromPos.column].removePiece();

		if (move.isCastling()) {
			int pieceRow = (pieceColor == Color::White) ? 7 : 0;
			if (toPos.column == 6) {
				board[toPos.row][toPos.column].addPiece(chess::createPiece(pieceType, pieceColor));
				board[toPos.row][7].removePiece();
				board[toPos.row][5].addPiece(chess::createPiece(chess::PieceType::Rook, pieceColor));
			}
			else {
				board[toPos.row][toPos.column].addPiece(chess::createPiece(pieceType, pieceColor));
				board[toPos.row][0].removePiece();
				board[toPos.row][3].addPiece(chess::createPiece(chess::PieceType::Rook, pieceColor));

			}
			castling.castlingFinished();
			GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, false, false, false, true);
			history.push_back(gameState);
			return;
		}
		if (move.isEnPassant()) {
			board[fromPos.row][toPos.column].removePiece();
			board[toPos.row][toPos.column].addPiece(chess::createPiece(pieceType, pieceColor));

			GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, true, true, false, false, pieceType);
			history.push_back(gameState);
			return;

		}
		if (pieceType == chess::PieceType::King) {
			if (pieceColor == Color::White) {
				castling.whiteKing = false;
				castling.whiteQueen = false;
			}
			else {
				castling.blackKing = false;
				castling.blackQueen = false;
			}
		}
		if (pieceType == chess::PieceType::Rook) {
			rookCastling(fromPos, pieceColor);
		}
		if (move.getPromotionPiece() == chess::PieceType::None) {
			if (board[toPos.row][toPos.column].hasPiece()) {
				chess::PieceType capturedPieceType = board[toPos.row][toPos.column].getPiece().getPieceType();
				if (capturedPieceType == chess::PieceType::Rook) {

					rookCastling(toPos, !pieceColor);
				}
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, true, false, false, false, capturedPieceType);
				history.push_back(gameState);
			}
			else {
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling);
				history.push_back(gameState);
			}
			board[toPos.row][toPos.column].addPiece(chess::createPiece(pieceType, pieceColor));
		}
		else {

			if (board[toPos.row][toPos.column].hasPiece()) {
				chess::PieceType capturedPieceType = board[toPos.row][toPos.column].getPiece().getPieceType();
				if (capturedPieceType == chess::PieceType::Rook) {

					rookCastling(toPos, !pieceColor);
				}
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, true, false, true, false, capturedPieceType);
				history.push_back(gameState);
			}
			else {
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, false, false, true, false);
				history.push_back(gameState);
			}
			board[toPos.row][toPos.column].addPiece(chess::createPiece(move.getPromotionPiece(), pieceColor));
		}
		//bitboard.loadBitboard(board);

	}
	void Board::unmakeMove() {
		GameState lastState = history.back();
		history.pop_back();
		sideToMove = !sideToMove;
		castling = lastState.previousCastlingRights;
		chess::Position fromPos = lastState.move.getFromPos();
		chess::Position toPos = lastState.move.getToPos();
		board[toPos.row][toPos.column].removePiece();
		if (lastState.enPassant) {
			board[fromPos.row][fromPos.column].addPiece(createPiece(chess::PieceType::Pawn, lastState.moveColor));
			board[fromPos.row][toPos.column].addPiece(createPiece(chess::PieceType::Pawn, !lastState.moveColor));

			return;
		}
		if (lastState.isPromotion) {
			board[fromPos.row][fromPos.column].addPiece(createPiece(chess::PieceType::Pawn, lastState.moveColor));

			if (lastState.isCapture) {
				board[toPos.row][toPos.column].addPiece(createPiece(lastState.capturedPiece, !lastState.moveColor));
			}
			return;
		}
		if (lastState.isCastling) {
			int pieceRow = (lastState.moveColor == Color::White) ? 7 : 0;
			if (toPos.column == 6) {
				board[fromPos.row][fromPos.column].addPiece(chess::createPiece(chess::PieceType::King, lastState.moveColor));

				board[toPos.row][5].removePiece();
				board[toPos.row][7].addPiece(chess::createPiece(chess::PieceType::Rook, lastState.moveColor));
			}
			else {
				board[fromPos.row][fromPos.column].addPiece(chess::createPiece(chess::PieceType::King, lastState.moveColor));
				board[toPos.row][3].removePiece();
				board[toPos.row][0].addPiece(chess::createPiece(chess::PieceType::Rook, lastState.moveColor));

			}
			return;
		}

		board[fromPos.row][fromPos.column].addPiece(chess::createPiece(lastState.playedPiece, lastState.moveColor));
		if (lastState.isCapture) {

			board[toPos.row][toPos.column].addPiece(chess::createPiece(lastState.capturedPiece, !lastState.moveColor));
		}

		//bitboard.loadBitboard(board);
	}
	bool Board::isCheck() {

		return bitboard.isCheck(sideToMove);
	}
	bool Board::isPinned() {
		//bitboard.g();
		return true;
	}
}
// Imam bitcode za sliding od kralja u svim direkacijama, onda isto imam i za kraljicu. Nakon toga provjerim gdje se sudaraju i to provjerim sa allPieces, ako je 

/*int moveCount = 1;
			if (piece->getColor() == Color::White) {
				if (row == 6) {
					moveCount = 2;
				}
			}
			else {
				if (row == 1) {
					moveCount = 2;
				}
			}
			for (int i = 1; i <= moveCount; i++) {
				if (piece->getColor() == Color::White) {
					if (isInside((row - i), col)) {
						if (!board[(row - i)][col].hasPiece()) {

							moves.push_back(Move(fromPos.row, fromPos.column, (row - i), col));
						}
						else {
							break;
						}
					}
				}
				else {
					if (isInside((row + i), col)) {
						if (!board[(row + i)][col].hasPiece()) {

							moves.push_back(Move(fromPos.row, fromPos.column, (row + i), col));
						}
						else {
							break;
						}
					}
				}
			}*/


			/*for (int i = 0; i < 2; i++) {
				if (i == 0) {
					int cRow = row;
					int cCol = col;;
					if (piece->getColor() == Color::White) {
						cRow--; cCol--;
					}
					else {
						cRow++; cCol--;
					}
					if (isInside(cRow, cCol) && board[cRow][cCol].hasPiece()) {
						if (board[cRow][cCol].getPiece()->getColor() != piece->getColor()) {

							moves.push_back(Move(fromPos.row, fromPos.column, cRow, cCol));
						}
					}
				}
				else {
					int cRow = row;
					int cCol = col;
					if (piece->getColor() == Color::White) {
						cRow--; cCol++;
					}
					else {
						cRow++; cCol++;
					}
					if (isInside(cRow, cCol) && board[cRow][cCol].hasPiece()) {
						if (board[cRow][cCol].getPiece()->getColor() != piece->getColor()) {

							moves.push_back(Move(fromPos.row, fromPos.column, cRow, cCol));
						}
					}
				}
			}*/
			/*if (row == 3 && piece->getColor() == Color::White) {
				for (int i = 0; i < 2; i++) {
					if (i == 0) {
						int cRow = row;
						int cCol = col - 1;
						if (isInside(cRow, cCol) && board[cRow][cCol].hasPiece()) {
							if (board[cRow][cCol].getPiece()->isPawn()) {

								if (history.back().getToPos() == chess::Position(cRow, cCol)) {
									moves.push_back(Move(fromPos.row, fromPos.column, cRow-1, cCol));
								}
							}
						}
					}
					else {
						int cRow = row;
						int cCol = col + 1;
						if (isInside(cRow, cCol) && board[cRow][cCol].hasPiece()) {
							if (board[cRow][cCol].getPiece()->isPawn()) {
								if (history.back().getToPos() == chess::Position(cRow, cCol)) {
									moves.push_back(Move(fromPos.row, fromPos.column, cRow-1, cCol));
								}
							}
						}
					}
				}
			}
			if (row == 4 && piece->getColor() == Color::Black) {
				for (int i = 0; i < 2; i++) {
					if (i == 0) {
						int cRow = row;
						int cCol = col - 1;
						if (isInside(cRow, cCol) && board[cRow][cCol].hasPiece()) {
							if (board[cRow][cCol].getPiece()->isPawn()) {
								if (history.back().getToPos() == chess::Position(cRow, cCol)) {
									moves.push_back(Move(fromPos.row, fromPos.column, cRow + 1, cCol));
								}
							}
						}
					}
					else {
						int cRow = row;
						int cCol = col + 1;
						if (isInside(cRow, cCol) && board[cRow][cCol].hasPiece()) {
							if (board[cRow][cCol].getPiece()->isPawn()) {
								if (history.back().getToPos() == chess::Position(cRow, cCol)) {
									moves.push_back(Move(fromPos.row, fromPos.column, cRow + 1, cCol));
								}
							}
						}
					}
				}
			}*/