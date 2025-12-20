#include<iostream>
#include <unordered_set>
#include <array>   

#include <string>
#include <vector>
#include "Board.h"
#include "../Move/Move.h"
#include "../Square/Square.h"
namespace chess {

	Board::Board() {
	//	board.resize(8);
		for (int r = 0; r < 8; r++) {
		//	board[r].resize(8);

			for (int c = 0; c < 8; c++) {
				if (r == 0 || r == 1 || r == 6 || r == 7) {
					if (r == 1) {
						board[r][c] = Square(r, c, Piece(Color::White, chess::PieceType::Pawn));
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

	//	board.resize(8);
		//for (int r = 0; r < 8; r++) {
	//		board[r].resize(8);
	//	}
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
		std::cout << std::endl;
	}
	bool Board::isInside(int r, int c) {
		return !(r > 7 || r < 0 || c>7 || c < 0);
	}

	void Board::makeMove(chess::Move move) {
		chess::Position fromPos = move.getFromPos();
		chess::Position toPos = move.getToPos();



		chess::PieceType pieceType = bitboard.getPieceType(fromPos);
		chess::Color pieceColor = bitboard.getColor(fromPos);
		chess::CastlingRights previousCastling = castling;
		auto k = move.getMoveText();
		sideToMove = !sideToMove;
	//	board[fromPos.row][fromPos.column].removePiece();

		if (move.isCastling()) {
			/*int pieceRow = (pieceColor == Color::White) ? 7 : 0;
			if (toPos.column == 6) {
				board[toPos.row][toPos.column].addPiece(chess::createPiece(pieceType, pieceColor));
				board[toPos.row][7].removePiece();
				board[toPos.row][5].addPiece(chess::createPiece(chess::PieceType::Rook, pieceColor));
			}
			else {
				board[toPos.row][toPos.column].addPiece(chess::createPiece(pieceType, pieceColor));
				board[toPos.row][0].removePiece();
				board[toPos.row][3].addPiece(chess::createPiece(chess::PieceType::Rook, pieceColor));

			}*/
			castling.castlingFinished();
			GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, false, false, false, true);
			bitboard.handleCastling(move, pieceColor);
			history.push_back(gameState);
			return;
		}
		if (move.isEnPassant()) {
			/*board[fromPos.row][toPos.column].removePiece();
			board[toPos.row][toPos.column].addPiece(chess::createPiece(pieceType, pieceColor));*/

			GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, true, true, false, false, pieceType);
			bitboard.handleEnPassant(move, pieceColor);
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
			if (bitboard.hasPiece(toPos)) {
				chess::PieceType capturedPieceType = bitboard.getPieceType(toPos);
				if (capturedPieceType == chess::PieceType::Rook) {

					rookCastling(toPos, !pieceColor);
				}
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, true, false, false, false, capturedPieceType);
				bitboard.makeMove(move, pieceType, pieceColor, true);
				history.push_back(gameState);
			}
			else {
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling);
				bitboard.makeMove(move, pieceType, pieceColor, false);
				history.push_back(gameState);
			}
			//board[toPos.row][toPos.column].addPiece(chess::createPiece(pieceType, pieceColor));
		}
		else {

			if (bitboard.hasPiece(toPos)) {
				chess::PieceType capturedPieceType = bitboard.getPieceType(toPos);
				if (capturedPieceType == chess::PieceType::Rook) {

					rookCastling(toPos, !pieceColor);
				}
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, true, false, true, false, capturedPieceType);
				bitboard.makeMove(move, pieceType, pieceColor, true);
				history.push_back(gameState);
			}
			else {
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, false, false, true, false);
				bitboard.makeMove(move, pieceType, pieceColor, false);
				history.push_back(gameState);
			}
			//board[toPos.row][toPos.column].addPiece(chess::createPiece(move.getPromotionPiece(), pieceColor));
		}
		//bitboard.loadBitboard(board);

	}
	void Board::unmakeMove() {
		GameState lastState = history.back();
		history.pop_back();
		sideToMove = !sideToMove;
		castling = lastState.previousCastlingRights;
		bitboard.unmakeMove(lastState.move,lastState.playedPiece,lastState.capturedPiece,lastState.moveColor);

		/*chess::Position fromPos = lastState.move.getFromPos();
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
		}*/

	}
	const int INF = 1e9;
	int Board::minimax_alpha_beta(int depth, int alpha, int beta) {
		if (depth == 0) {
			return evaluate();
			//return quiescence_search(alpha, beta);
		}
		MoveList moves = getAllLegalMoves();

		if (moves.count == 0) {
			if (isCheck()) {
			//	printBoard();
				return (sideToMove == Color::White) ? (-INF - depth) : (INF + depth);
			}
			return 0;
		}
		if (sideToMove == Color::White) {
			int maxEval = -INF;
			for (Move mv : moves) {
				makeMove(mv);
				int eval = minimax_alpha_beta(depth - 1, alpha, beta);
				unmakeMove();
				maxEval = std::max(maxEval, eval);
				alpha = std::max(alpha, eval);
				if (beta <= alpha) break;
			}
			return maxEval;
		}
		else {
			int minEval = INF;
			for (Move mv : moves) {
				makeMove(mv);
				int eval = minimax_alpha_beta(depth - 1, alpha, beta);
				unmakeMove();
				minEval = std::min(minEval, eval);
				beta = std::min(beta, eval);
				if (beta <= alpha) break;
			}
			return minEval;
		}
	};

	int Board::quiescence_search(int alpha, int beta) {
		int stand_pat = evaluate();

		if (stand_pat >= beta) {
			return beta; 
		}

		if (stand_pat > alpha) {
			alpha = stand_pat;
		}

		MoveList captures = getAllLegalCaptures();


		for (const auto& mv : captures) {

			makeMove(mv);
			int score = -quiescence_search(-beta, -alpha);
			unmakeMove();


			if (score >= beta) {
				return beta;
			}
			if (score > alpha) {
				alpha = score; 
			}
		}

		return alpha;
	}
	Move Board::findBestMove(int depth) {
		int bestVal = (sideToMove == Color::White) ? -INF : INF;
		Move bestMove;
		int alpha = -INF;
		int beta = INF;
		for (auto mv : getAllLegalMoves()) {
			makeMove(mv);
			int moveVal = minimax_alpha_beta(depth, alpha, beta);
			if (isCheckMate()) {
				unmakeMove();

				return mv;
			}
			unmakeMove();
			if ((sideToMove == Color::White)) {

				if (moveVal > bestVal) {
					bestMove = mv;
					bestVal = moveVal;
					alpha = std::max(alpha, bestVal);
				}
			}
			else {
				if (moveVal < bestVal) {
					bestMove = mv;
					bestVal = moveVal;
					beta = std::min(beta, bestVal);
				}
			}

		}
		return bestMove;
	}
}