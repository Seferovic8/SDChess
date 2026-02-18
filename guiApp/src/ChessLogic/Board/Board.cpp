#include<iostream>
#include <unordered_set>
#include <array>   
#include <algorithm>
#include <random>
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
		check = Check::Undefined;

		sideToMove = Color::White;
		bitboard.loadBitboard(board, sideToMove);
	}
	Board::Board(std::string fen) {

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
		check = Check::Undefined;
	}
	std::vector< std::vector<Square>> Board::getBoard() {
		buildFromBitboards();

		std::vector< std::vector<Square>> result;
		result.reserve(8);

		for (int rank = 0; rank < 8; ++rank) {
			std::vector< Square> row;
			row.reserve(8);
			for (int file = 0; file < 8; ++file) {
				row.push_back(board[rank][file]);
			}
			result.push_back(row);
		}

		return result;
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
	chess::Square Board::getSquare(chess::Position pos) {
		buildFromBitboards();
		return board[pos.row][pos.column];
	}
	bool Board::isRepetition() {
		if(history.empty())
			return false;

		uint64_t currentHash = history.back().previousBoardState;
		int count = 1;

		for (int i = history.size() - 2; i >= 0; i--)
		{
			if (history[i].previousBoardState == currentHash)
			{
				count++;
				if (count >= 3)
					return true;
			}
		}

		return false;
	}
	void Board::buildFromBitboards()
	{
		// 1️⃣ Clear board
		for (int r = 0; r < 8; ++r) {
			for (int c = 0; c < 8; ++c) {
				board[r][c] = chess::Square(r, c);
			}
		}

		// 2️⃣ Iterate all occupied squares
		uint64_t all = bitboard.getAllPieces();

		while (all) {
			int sq = Bitboard::lsb(all);
			all &= all - 1; // pop lsb

			chess::Position pos(sq);
			int row = pos.row;
			int col = pos.column;

			PieceType pt = bitboard.getPieceType(sq);
			Color color = bitboard.getColor(sq);

			board[row][col] = Square(row, col, Piece(color, pt));
		}
	}

	void Board::makeMove(chess::Move move) {
		int fromPos = move.getFromSq();
		int toPos = move.getToSq();
		check = Check::Undefined;


		chess::PieceType pieceType = bitboard.getPieceType(fromPos);
		chess::Color pieceColor = bitboard.getColor(fromPos);
		chess::CastlingRights previousCastling = castling;

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
			castling.castlingFinished(pieceColor);
			GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, bitboard.getAllPieces(), false, false, false, true);
			bitboard.handleCastling(move, pieceColor);
			history.push_back(gameState);
			return;
		}
		if (move.isEnPassant()) {
			/*board[fromPos.row][toPos.column].removePiece();
			board[toPos.row][toPos.column].addPiece(chess::createPiece(pieceType, pieceColor));*/

			GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, bitboard.getAllPieces(), true, true, false, false, pieceType);
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
			if (bitboard.hasPiece(move.getToSq())) {
				chess::PieceType capturedPieceType = bitboard.getPieceType(toPos);
				if (capturedPieceType == chess::PieceType::Rook) {

					rookCastling(toPos, !pieceColor);
				}
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, bitboard.getAllPieces(), true, false, false, false, capturedPieceType);
				bitboard.makeMove(move, pieceType, pieceColor, true);
				history.push_back(gameState);
			}
			else {
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, bitboard.getAllPieces());
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
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, bitboard.getAllPieces(), true, false, true, false, capturedPieceType);
				bitboard.makeMove(move, pieceType, pieceColor, true);
				history.push_back(gameState);
			}
			else {
				GameState gameState = GameState(move, pieceType, !sideToMove, previousCastling, bitboard.getAllPieces(), false, false, true, false);
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
		check = Check::Undefined;

		castling = lastState.previousCastlingRights;
		bitboard.unmakeMove(lastState.move, lastState.playedPiece, lastState.capturedPiece, lastState.moveColor);

	}
	const int INF = 1e9;
	int Board::minimax_alpha_beta(int depth, int alpha, int beta) {
		if (isRepetition()) {
			return (sideToMove == Color::White) ? -50 : 50;
		}
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
		int stand_pat = evaluate(); // Assumes evaluate() returns Absolute score (White > 0, Black < 0)

		// --- WHITE (Maximizing) ---
		if (sideToMove == Color::White) {
			if (stand_pat >= beta) {
				return beta;
			}
			if (stand_pat > alpha) {
				alpha = stand_pat;
			}

			MoveList captures = getAllLegalCaptures();
			for (const auto& mv : captures) {
				makeMove(mv);
				// Call recursively with same alpha/beta (Minimax style), NO negation
				int score = quiescence_search(alpha, beta);
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
		// --- BLACK (Minimizing) ---
		else {
			// Black wants low scores. If stand_pat is already lower than alpha (White's best guaranteed),
			// White won't let us get here. But for pruning:
			// Alpha is the MAX White can force. Beta is the MIN Black can force.

			if (stand_pat <= alpha) {
				return alpha;
			}
			if (stand_pat < beta) {
				beta = stand_pat;
			}

			MoveList captures = getAllLegalCaptures();
			for (const auto& mv : captures) {
				makeMove(mv);
				// Call recursively with same alpha/beta
				int score = quiescence_search(alpha, beta);
				unmakeMove();

				if (score <= alpha) {
					return alpha;
				}
				if (score < beta) {
					beta = score;
				}
			}
			return beta;
		}
	}
	std::pair<Move,int> Board::findBestMove(int depth) {
		int bestVal = (sideToMove == Color::White) ? -INF + 1 : INF - 1;
		Move bestMove;
		int alpha = -INF;
		int beta = INF;
		for (auto mv : getAllLegalMoves()) {
			makeMove(mv);
			int moveVal = minimax_alpha_beta(depth, alpha, beta);
			if (isCheckMate()) {
				unmakeMove();

				return { mv ,moveVal};
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
		return { bestMove ,bestVal };
	}
	 std::string Board::generateChess960FEN()
	{
		std::vector<char> backRank(8, ' ');
		std::vector<int> freeSquares = { 0,1,2,3,4,5,6,7 };

		std::random_device rd;
		std::mt19937 gen(rd());

		// 1️⃣ Place bishops on opposite colors
		std::vector<int> darkSquares = { 0,2,4,6 };
		std::vector<int> lightSquares = { 1,3,5,7 };

		std::uniform_int_distribution<> darkDist(0, darkSquares.size() - 1);
		std::uniform_int_distribution<> lightDist(0, lightSquares.size() - 1);

		int b1 = darkSquares[darkDist(gen)];
		int b2 = lightSquares[lightDist(gen)];

		backRank[b1] = 'B';
		backRank[b2] = 'B';

		freeSquares.erase(std::remove(freeSquares.begin(), freeSquares.end(), b1), freeSquares.end());
		freeSquares.erase(std::remove(freeSquares.begin(), freeSquares.end(), b2), freeSquares.end());

		// 2️⃣ Place queen
		std::uniform_int_distribution<> distQ(0, freeSquares.size() - 1);
		int qIndex = freeSquares[distQ(gen)];
		backRank[qIndex] = 'Q';

		freeSquares.erase(std::remove(freeSquares.begin(), freeSquares.end(), qIndex), freeSquares.end());

		// 3️⃣ Place knights
		std::shuffle(freeSquares.begin(), freeSquares.end(), gen);
		backRank[freeSquares[0]] = 'N';
		backRank[freeSquares[1]] = 'N';

		freeSquares.erase(freeSquares.begin(), freeSquares.begin() + 2);

		// 4️⃣ Place rooks and king (king between rooks)
		std::sort(freeSquares.begin(), freeSquares.end());

		backRank[freeSquares[0]] = 'R';
		backRank[freeSquares[1]] = 'K';
		backRank[freeSquares[2]] = 'R';

		// Build back rank string
		std::string whiteBackRank(backRank.begin(), backRank.end());

		// Build black back rank (lowercase)
		std::string blackBackRank;
		for (char c : whiteBackRank)
			blackBackRank += std::tolower(c);

		// Construct full FEN
		std::string fen =
			blackBackRank + "/pppppppp/8/8/8/8/PPPPPPPP/" +
			whiteBackRank + " w - 0 1";

		return fen;
	}
}