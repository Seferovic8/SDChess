#pragma once
#include "Board/Board.h"
#include "Position/Position.h"
#include "Square/Square.h"
enum class ChessType {
    Regular,
    Chess960
};
enum class ChessColor {
    White,
    Random,
    Black
};
enum class ChessDifficulty {
    Easy,
    Medium,
    Hard
};