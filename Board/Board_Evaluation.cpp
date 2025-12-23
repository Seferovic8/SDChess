#include "Board.h"
#include "evaluation_parameters.cpp"
namespace chess {
	int Board::evaluate(){
		bitboard.updateMaterial();
		int whiteMaterial =getMaterialScore(Color::White);
		int blackMaterial =getMaterialScore(Color::Black);

		int whitePieceSquare = getPositionScore(Color::White);
		int blackPieceSquare = getPositionScore(Color::Black);

		int score = (whiteMaterial - blackMaterial)+
			(whitePieceSquare - blackPieceSquare);
		return score;
	}
	int Board::getMaterialScore(Color color) {
		int score = 0;
		for (int i = 0; i < 5; i++) {
			score += bitboard.material[static_cast<int>(color)][i].n * weights.MaterialValue[i];
		}
		return score;
	}
	int Board::getMobilityScore(Color color) {
		int score = 0;
		for (int i = 1; i < 6; i++) {
			for (int position : bitboard.material[static_cast<int>(color)][i]) {
				//MoveList moves = generatePieceMoves(chess::Position(position));
				score += 0;
			}
		}
		return score;
	}
	int Board::getPositionScore(Color color) {
		int score = 0;
		for (int i = 1; i < 6; i++) {
			for (int position : bitboard.material[static_cast<int>(color)][i]) {
				if (color == Color::Black) {
					position = chess::Position::flipPosition(position);
				}
				score += weights.PieceSquareTables[i][position]; 
				auto ba = bitboard.material[static_cast<int>(color)][i]; 
			}
		}
		return score;
	}
}