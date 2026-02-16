#include "Board.h"
#include "evaluation_parameters.cpp"
namespace chess {
	//int Board::evaluate(){
	//	bitboard.updateMaterial();
	//	int whiteMaterial =getMaterialScore(Color::White);
	//	int blackMaterial =getMaterialScore(Color::Black);

	//	int positionalScore = getPositionalScore();

	//	int score = (whiteMaterial - blackMaterial)+
	//		positionalScore;
	//	return score;
	//}
	int Board::evaluate() {
		bitboard.updateMaterial();
		// 1. Initialize Scores
		int mgScore = 0;
		int egScore = 0;
		int phase = 0;

		// 2. Single Loop for everything (Efficiency)
		// Loop through Piece Types: Pawn(0) to King(5)
		for (int pt = 0; pt < 6; pt++) {
			int matVal = weights.MaterialValue[pt];
			int phaseVal = weights.PHASE_WEIGHTS[pt];

			// Loop through Colors: White(0), Black(1)
			for (int c = 0; c < 2; c++) {
				Color color = static_cast<Color>(c);

				// Loop through all pieces of this type/color
				for (int sq : bitboard.material[c][pt]) {

					// --- A. PHASE ---
					phase += phaseVal;

					// --- B. PST & MATERIAL ---
					// If Black, flip square for PST
					int tableSquare = (color == Color::White) ? sq : chess::Position::flipPosition(sq);

					// Get PST values
					int pstMg = weights.mg_pesto_table[pt][tableSquare];
					int pstEg = weights.eg_pesto_table[pt][tableSquare];

					// Add to Global Scores
					// White ADDS, Black SUBTRACTS (Absolute Calculation)
					if (color == Color::White) {
						mgScore += (matVal + pstMg);
						egScore += (matVal + pstEg);
					}
					else {
						mgScore -= (matVal + pstMg);
						egScore -= (matVal + pstEg);
					}
				}
			}
		}

		// 3. Tapered Evaluation
		int totalPhase = weights.TOTAL_PHASE;
		if (phase > totalPhase) phase = totalPhase;

		// Interpolate between Middlegame and Endgame
		int finalScore = ((mgScore * phase) + (egScore * (totalPhase - phase))) / totalPhase;

		return finalScore;
		//return (bitboard.sideToMove == Color::Black) ? finalScore : -finalScore;
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
	int Board::getPositionalScore() {
		std::array<int, 3> whitePieceSquare = getPestoScore(Color::White);
		std::array<int, 3> blackPieceSquare = getPestoScore(Color::Black);
		int mgScore = whitePieceSquare[0] - blackPieceSquare[0];
		int egScore = whitePieceSquare[1] - blackPieceSquare[1];
		int phase = whitePieceSquare[2] + blackPieceSquare[2];
		if (phase > weights.TOTAL_PHASE) phase = weights.TOTAL_PHASE;
		int finalScore = ((mgScore * phase) + (egScore * (weights.TOTAL_PHASE - phase))) / weights.TOTAL_PHASE;

		// Return relative to side to move
		return (sideToMove == Color::White) ? finalScore : -finalScore;

	}
	std::array<int, 3> Board::getPestoScore(Color color) {
		int mgScore = 0;
		int egScore = 0;
		int phase = 0;
		for (int i = 0; i < 6; i++) {
			for (int position : bitboard.material[static_cast<int>(color)][i]) {
				if (color == Color::Black) {
					position = chess::Position::flipPosition(position);
				}
				int mgVal = weights.mg_pesto_table[i][position];
				int egVal = weights.eg_pesto_table[i][position];
					mgScore += mgVal;
					egScore += egVal;


				// 2. Update Game Phase
				phase += weights.PHASE_WEIGHTS[i];
			}
		}
		return { mgScore,egScore,phase };
	}
}