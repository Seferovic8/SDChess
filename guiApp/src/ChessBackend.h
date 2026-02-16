#include <iostream>
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <td/Types.h>
#include <gui/Canvas.h>
#include <thread>
#include <mutex>
#include <gui/Thread.h>
#include <thread/Thread.h>
#include <gui/Image.h>
#include <gui/Shape.h>
#include <gui/Application.h>
#include <gui/DrawableString.h>
#include "chessLogic/_all.h"
#include <cmath>
#ifdef _WIN32
#include <windows.h>
#include <Lmcons.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif
#include "ChessTimer.h"

constexpr size_t N = 8;

inline int snapAwayFromZero(float x) {
	return (x > 0) ? int(std::ceil(x)) :
		(x < 0) ? int(std::floor(x)) :
		0;
}
enum class GameResult { Ongoing, WhiteWinCheckMate, BlackWinCheckMate, WhiteWinTimeout, BlackWinTimeout, Draw };
struct RectCoordinates
{
	float left = 0;
	float top = 0;
	float right = 0;
	float bottom = 0;
	RectCoordinates() = default;

	RectCoordinates(float l, float t, float r, float b)
		: left(l), top(t), right(r), bottom(b) {
	}
};
struct MoveEntry {
	int moveNumber;
	std::string whiteMove;
	std::string blackMove;
};
struct PositionHash {
	std::size_t operator()(const chess::Position& p) const noexcept {
		return (std::size_t(p.row) << 3) | std::size_t(p.column);
	}
};
class ChessBackend
{
public:
	int evalScore = 0;
	chess::Color _playerColor;
	enum class UserMove : td::BYTE { None = 0, ClickedBK, DragBK };
	using PromotionCallback =
		std::function<void(chess::Color, const gui::Point& modelPoint)>;
private:
	std::string _assets = "../assets/";
	gui::Image _wk;
	gui::Image _bk;
	gui::Image _wp;
	gui::Image _bp;
	gui::Image _wq;
	gui::Image _bq;
	gui::Image _wr;
	gui::Image _br;
	gui::Image _wb;
	gui::Image _bb;
	gui::Image _wn;
	gui::Image _bn;
	std::unordered_map<chess::Position, RectCoordinates, PositionHash>squareCordinates;
	std::unordered_map<chess::PieceEncoding, gui::Image*> pieceImages = {
		{ chess::PieceEncoding::WhiteKing, &_wk},
		{ chess::PieceEncoding::BlackKing,&_bk},
		{ chess::PieceEncoding::WhiteQueen, &_wq},
		{ chess::PieceEncoding::BlackQueen, &_bq},
		{ chess::PieceEncoding::WhiteRook, &_wr},
		{ chess::PieceEncoding::BlackRook, &_br},
		{ chess::PieceEncoding::WhitePawn, &_wp},
		{ chess::PieceEncoding::BlackPawn, &_bp},
		{ chess::PieceEncoding::WhiteKnight, &_wn},
		{ chess::PieceEncoding::BlackKnight, &_bn},
		{ chess::PieceEncoding::WhiteBishop, &_wb},
		{ chess::PieceEncoding::BlackBishop, &_bb},
	};
	gui::Image _imgBoard;
	std::thread _workingThread;
	std::mutex _mutex;
	gui::Rect _boardPlacement;
	gui::Size _boardImgSize;
	gui::Point _ptOrig;
	gui::Point _dragStartPoint;
	gui::Point _currentDragPoint;
	gui::Size _cellSize;
	const float _originXMargin = 0.045f;   //relative to biard width, after fine-tuning
	const float _originYMargin = 0.045f;    //relative to board height
	UserMove _userMove = UserMove::None;
	bool _calcBoardPlacement = true;
	PromotionCallback _promotionCallback;
	std::function<void()> _resetCallback;
	ChessDifficulty _chessDifficulty;
	chess::Board _board;
	gui::Size _viewSize;
	std::string _userName;
	int _depth;
	ChessTimer white;
	ChessTimer black;
	gui::Size imgSize;
	gui::Point imgPt;
	std::vector<MoveEntry> _moveHistory;
	bool _showEvaluationBar = false;
	RectCoordinates evaluationBarButton;
	RectCoordinates _newGameBtn;
	std::vector<std::vector<chess::Square>> chessBoard;
public:
	bool reDrawBoard = true;
	bool winnerPanelDraw = false;

private:

	void addMoveToHistory(chess::Move& move)
	{
		std::string moveStr = _board.moveToSAN(move);

		if (!_board.sideToMove == chess::Color::Black)
		{
			MoveEntry entry;
			entry.moveNumber = _moveHistory.size() + 1;
			entry.whiteMove = moveStr;
			entry.blackMove = "";
			_moveHistory.push_back(entry);
		}
		else
		{
			if (!_moveHistory.empty())
				_moveHistory.back().blackMove = moveStr;
		}
	}


	void getCellRect(gui::Rect& rect, chess::Position pos)
	{
		gui::Point tl(_ptOrig);

		tl.x += pos.column * _cellSize.width;
		tl.y -= (pos.row) * _cellSize.height;
		rect.left = tl.x;
		rect.top = tl.y;
		rect.right = tl.x + _cellSize.width;
		rect.bottom = tl.y + _cellSize.height;
		squareCordinates.insert_or_assign(pos,
			RectCoordinates(rect.left, rect.top, rect.right, rect.bottom));
	}
	void getCellCircle(gui::Circle& circle, chess::Position pos)
	{
		gui::Point tl(_ptOrig);

		tl.x += pos.column * _cellSize.width;
		tl.y -= (pos.row) * _cellSize.height;
		circle.center.x = tl.x + _cellSize.width / 2;
		circle.center.y = tl.y + _cellSize.height / 2;
	}

	chess::Position getCellCoordinate(const gui::Point& cursorPoint) const
	{
		for (auto& [key, value] : squareCordinates) {
			if (value.left <= cursorPoint.x && cursorPoint.x <= value.right &&
				value.top <= cursorPoint.y && cursorPoint.y <= value.bottom) {
				return _playerColor == chess::Color::White ? chess::Position::flipPosition(key) : chess::Position::flipHorizontaly(key);
			}
		}
		return chess::Position(-1, -1);

	}

public:
	ChessBackend(ChessDifficulty chessDifficulty,bool isChess960, chess::Color playerColor, PromotionCallback promotionCallback, std::function<void()> resetCallback)
		: white(15), black(15), _chessDifficulty(chessDifficulty), _imgBoard(playerColor == chess::Color::White ? "../assets/board.jpg" : "../assets/board-flipped.png")
		, _wk(_assets + "wk.png")
		, _bk(_assets + "bk.png")
		, _wp(_assets + "wp.png")
		, _bp(_assets + "bp.png")
		, _wq(_assets + "wq.png")
		, _bq(_assets + "bq.png")
		, _wr(_assets + "wr.png")
		, _br(_assets + "br.png")
		, _wn(_assets + "wn.png")
		, _bn(_assets + "bn.png")
		, _wb(_assets + "wb.png")
		, _bb(_assets + "bb.png")
		, _playerColor(playerColor)
		, _promotionCallback(std::move(promotionCallback))
		, _resetCallback(std::move(resetCallback))
		, _board(isChess960?chess::Board::generateChess960FEN() : "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
	{
#ifdef _WIN32

		char username[UNLEN + 1];
		DWORD username_len = UNLEN + 1;

		if (GetUserNameA(username, &username_len))
			_userName = std::string(username);
		else
			_userName = "Player";

#else

		struct passwd* pw = getpwuid(getuid());
		if (pw)
			_userName = std::string(pw->pw_name);
		else
			_userName = "Player";

#endif
		//	reset();
		_imgBoard.getSize(_boardImgSize);

		updateImgSizes();
		timerTick(chess::Color::White);
	}



	void timerTick(chess::Color color)
	{
		if (color == chess::Color::White) {
			white.start();
			black.pause();
		}
		else {
			black.start();
			white.pause();
		}

	}

	void reset()
	{
		_resetCallback();
	}
	std::pair<int, int> splitByEval(int size, int eval, int evalRange = 1800)
	{
		eval = std::clamp(eval, -evalRange, evalRange);

		int half = size / 2;

		// Normalize eval to [-1, 1]
		double factor = static_cast<double>(eval) / evalRange;

		// Max shift allowed (cannot exceed half)
		int shift = static_cast<int>(factor * half);

		int part1 = half + shift;
		int part2 = size - part1;

		return { part1, part2 };
	}
	void drawEvaluationBar() {
		int margin = 25;
		auto evalResult = splitByEval(_viewSize.height - 2 * margin, evalScore);
		gui::Shape _evalBar;
		gui::Point upperPt(imgPt.x - 50, margin);
		gui::Rect r(upperPt, gui::Size(30, _viewSize.height - 2 * margin));
		_evalBar.createRect(r, 2.0f);
		_evalBar.drawFill((_playerColor == chess::Color::White) ? td::ColorID::White : td::ColorID::Black);

		auto upperSz = gui::Size(30, (_playerColor == chess::Color::White) ? evalResult.second : evalResult.first);
		gui::Rect upperRectangle(upperPt, upperSz);
		_evalBar.createRect(upperRectangle, 2.0f);
		_evalBar.drawFill((_playerColor == chess::Color::White) ? td::ColorID::Black : td::ColorID::White);
	}
	void drawPlayerTime(chess::Color cl) {

		gui::DrawableString playerStr((cl == _playerColor) ? _userName : "Engine");
		gui::Image playerIcon((cl == _playerColor) ? "../assets/playerImage.png" : "../assets/engine.png");
		gui::Point upperPt(imgPt.x, imgPt.y - 50);
		gui::Size timeSz(130, 40);
		gui::Point timePt(imgPt.x + imgSize.width - timeSz.width, imgPt.y - 50);
		auto timeColor = (cl == chess::Color::Black) ? td::ColorID::ObsidianGray : td::ColorID::LightGray;
		if (white.isRunning() && (cl == chess::Color::White))
			timeColor = td::ColorID::White;
		if (black.isRunning() && (cl == chess::Color::Black))
			timeColor = td::ColorID::Black;
		if ((cl == _playerColor)) {
			upperPt.y = imgPt.y + imgSize.height + 10;
			timePt.y = imgPt.y + imgSize.height + 10;
		}
		gui::Font _fontExplanation;
		_fontExplanation.create("Roboto", 12.0f, gui::Font::Style::Bold, gui::Font::Unit::LogicalPixel);

		gui::Shape _playerBar;
		gui::Rect r(upperPt, gui::Size(100, 40));
		playerIcon.draw(r, gui::Image::AspectRatio::Keep, td::HAlignment::Left, td::VAlignment::Center);
		playerStr.draw(r, &_fontExplanation, td::ColorID::White, td::TextAlignment::Right, td::VAlignment::Center);

		gui::Rect timeRect(timePt, timeSz);
		gui::DrawableString timeStr((cl == chess::Color::Black) ? black.getFormattedTime() : white.getFormattedTime());
		_playerBar.createRoundedRect(timeRect, 5, 3.0f);;
		_playerBar.drawFill(timeColor);

		timeStr.draw(timeRect, &_fontExplanation, (cl == chess::Color::Black) ? td::ColorID::White : td::ColorID::ObsidianGray, td::TextAlignment::Center, td::VAlignment::Center);
	}
	GameResult checkGameResult()
	{
		bool whiteTimeout = white.isTimeUp();
		bool blackTimeout = black.isTimeUp();
		if (whiteTimeout) {

			winnerPanelDraw = true;
			return GameResult::BlackWinTimeout;
		}
		if (blackTimeout) {

			winnerPanelDraw = true;
			return GameResult::WhiteWinTimeout;
		}
		if (_board.isCheckMate())
		{
			winnerPanelDraw = true;
			if (_board.sideToMove == chess::Color::White)
				return GameResult::BlackWinCheckMate;
			else
				return GameResult::WhiteWinCheckMate;
		}
		if (_board.getAllLegalMoves().count == 0) {

			winnerPanelDraw = true;
			return GameResult::Draw;
		}
		_newGameBtn = RectCoordinates(-1, -1, -1, -1);
		winnerPanelDraw = false;

		return GameResult::Ongoing;
	}

	void drawButton(const gui::Rect& rect)
	{
		gui::Font font;
		font.create("Roboto", 12.0f,
			gui::Font::Style::Normal,
			gui::Font::Unit::LogicalPixel);

		td::ColorID topStripColor = _showEvaluationBar ? td::ColorID::Green : td::ColorID::Red;

		gui::Size btnSz(50, 25);
		gui::Point btnPt(imgPt.x + imgSize.width + 40, imgPt.y - btnSz.height - 10);

		gui::Shape button;
		gui::Rect btnRect(
			btnPt,
			btnSz
		);
		evaluationBarButton = RectCoordinates(btnRect.left, btnRect.top, btnRect.right, btnRect.bottom);
		button.createRoundedRect(btnRect, 10, 0);
		button.drawFill(td::ColorID::ObsidianGray);

		gui::Point circleCenter(_showEvaluationBar ? (btnRect.right - btnSz.width / 3.8) : (btnRect.left + btnSz.width / 3.8), btnRect.top + btnSz.height / 2);
		gui::Circle btnCircle(circleCenter, 9);
		button.createCircle(btnCircle, 0);
		button.drawFill(topStripColor);

		gui::Rect textRect(
			gui::Point(btnRect.right + 15, btnPt.y),
			gui::Size(btnSz.width + 150, btnSz.height)
		);
		gui::DrawableString str("Show Evaluation bar");
		str.draw(textRect,
			&font,
			td::ColorID::White,
			td::TextAlignment::Left,
			td::VAlignment::Center);
	}
	void drawMoves(const gui::Rect&)
	{
		gui::Font moveFont;
		gui::Font moveNumberFont;

		const int rowHeight = 28;
		const int leftPadding = 16;
		const int columnSpacing = 80;

		moveFont.create("Segoe UI", 15.0f,
			gui::Font::Style::Normal,
			gui::Font::Unit::LogicalPixel);

		moveNumberFont.create("Segoe UI", 15.0f,
			gui::Font::Style::Bold,
			gui::Font::Unit::LogicalPixel);

		gui::Rect panelRect(
			imgPt.x + imgSize.width + 40,
			imgPt.y,
			_viewSize.width - 20,
			imgPt.y + imgSize.height
		);

		gui::Shape panel;
		panel.createRect(panelRect, 0);
		panel.drawFill(td::ColorID::ObsidianGray);

		// 🔥 SCROLL LOGIC
		int maxVisibleRows = panelRect.height() / rowHeight;
		int totalRows = static_cast<int>(_moveHistory.size());

		int startIndex = 0;

		if (totalRows > maxVisibleRows)
			startIndex = totalRows - maxVisibleRows;

		int currentY = panelRect.top;

		for (int i = startIndex; i < totalRows; i++)
		{
			const auto& entry = _moveHistory[i];

			gui::Rect rowRect(
				panelRect.left,
				currentY,
				panelRect.right,
				currentY + rowHeight
			);
			panel.createRect(rowRect, 0);
			// Alternating row color (based on REAL index)
			if (i % 2 == 0)
				panel.drawFill(td::ColorID::Black);
			else
				panel.drawFill(td::ColorID::ObsidianGray);

			// Move number
			gui::Rect numberRect(
				rowRect.left + leftPadding,
				rowRect.top,
				rowRect.left + leftPadding + 35,
				rowRect.bottom
			);

			gui::DrawableString moveNumberStr(std::to_string(entry.moveNumber) + ".");
			moveNumberStr.draw(numberRect,
				&moveNumberFont,
				td::ColorID::LightGray,
				td::TextAlignment::Left,
				td::VAlignment::Center);

			// White move
			gui::Rect whiteRect(
				rowRect.left + leftPadding + 40,
				rowRect.top,
				rowRect.left + leftPadding + 40 + columnSpacing,
				rowRect.bottom
			);

			gui::DrawableString whiteStr(entry.whiteMove);
			whiteStr.draw(whiteRect,
				&moveFont,
				td::ColorID::White,
				td::TextAlignment::Left,
				td::VAlignment::Center);

			// Black move
			gui::Rect blackRect(
				rowRect.left + leftPadding + 40 + columnSpacing,
				rowRect.top,
				rowRect.left + leftPadding + 40 + 2 * columnSpacing,
				rowRect.bottom
			);

			gui::DrawableString blackStr(entry.blackMove);
			blackStr.draw(blackRect,
				&moveFont,
				td::ColorID::White,
				td::TextAlignment::Left,
				td::VAlignment::Center);

			// Separator line
			gui::Shape::drawLine(
				{ rowRect.left + 10, rowRect.bottom },
				{ rowRect.right - 10, rowRect.bottom },
				td::ColorID::DarkGray,
				1.0f
			);

			currentY += rowHeight;
		}
	}
	void drawWinner(const gui::Rect& rect)
	{
		std::string mainTitle;
		std::string subTitle;
		td::ColorID titleColor = td::ColorID::Red;
		auto gameResult = checkGameResult();

		// TIMEOUT
		if (gameResult == GameResult::WhiteWinTimeout || gameResult == GameResult::BlackWinTimeout)
		{
			if (gameResult == GameResult::BlackWinTimeout)
			{
				mainTitle = "BLACK WINS!";
				titleColor = _playerColor == chess::Color::Black ? td::ColorID::Green : td::ColorID::Red;

			}
			else
			{
				mainTitle = "WHITE WINS!";
				titleColor = _playerColor == chess::Color::White ? td::ColorID::Green : td::ColorID::Red;

			}

			subTitle = "By Timeout";
		}
		// CHECKMATE
		else if (gameResult == GameResult::WhiteWinCheckMate)
		{

			mainTitle = "WHITE WINS!";

			subTitle = "By Checkmate";
			titleColor = _playerColor == chess::Color::White ? td::ColorID::Green : td::ColorID::Red;
		}
		else if (gameResult == GameResult::BlackWinCheckMate)
		{

			mainTitle = "BLACK WINS!";

			subTitle = "By Checkmate";
			titleColor = _playerColor == chess::Color::Black ? td::ColorID::Green : td::ColorID::Red;
		}
		// STALEMATE
		else if (gameResult == GameResult::Draw)
		{
			mainTitle = "DRAW!";
			subTitle = "Stalemate";
			titleColor = td::ColorID::Yellow;
		}

		gui::Shape shape;
		gui::Font _font;

		_font.create("Roboto", 36.0f, gui::Font::Style::Bold, gui::Font::Unit::LogicalPixel);

		gui::Size boxSize(500, 350);
		gui::Point boxPt(
			rect.width() / 2 - boxSize.width / 2,
			rect.height() / 2 - boxSize.height / 2
		);

		gui::Rect boxRect(boxPt, boxSize);

		shape.createRoundedRect(boxRect, 15);
		shape.drawFillAndWire(td::ColorID::Bisque, titleColor, 4);

		// TITLE
		gui::Rect titleRect(
			gui::Point(boxPt.x, boxPt.y + 60),
			gui::Size(boxSize.width, 100)
		);

		gui::DrawableString titleStr(mainTitle);
		titleStr.draw(titleRect,
			&_font,
			titleColor,
			td::TextAlignment::Center,
			td::VAlignment::Center);
		// SUBTITLE
		gui::Rect subRect(
			gui::Point(boxPt.x, boxPt.y + 140),
			gui::Size(boxSize.width, 50)
		);

		gui::DrawableString subStr(subTitle);
		subStr.draw(subRect,
			gui::Font::ID::SystemBold,
			td::ColorID::Gray,
			td::TextAlignment::Center,
			td::VAlignment::Center);

		// TRY AGAIN BUTTON
		gui::Size btnSize(boxSize.width - 100, 70);
		gui::Point tryPt(
			rect.width() / 2 - btnSize.width / 2,
			boxPt.y + 230
		);

		gui::Rect newGameRect(tryPt, btnSize);
		_newGameBtn = RectCoordinates(newGameRect.left, newGameRect.top, newGameRect.right, newGameRect.bottom);

		shape.createRoundedRect(newGameRect, 8);
		shape.drawFillAndWire(td::ColorID::Green, td::ColorID::Gold, 2);
		gui::Font _newGamefont;

		_newGamefont.create("Roboto", 20.0f, gui::Font::Style::Normal, gui::Font::Unit::LogicalPixel);
		gui::DrawableString newGameStr("New game");
		newGameStr.draw(newGameRect,
			&_newGamefont,
			td::ColorID::White,
			td::TextAlignment::Center,
			td::VAlignment::Center);

	}
	void draw()
	{

		float xCenter = (_viewSize.width / 2);
		float yCenter = (_viewSize.height / 2);
		gui::Rect imgRect(imgPt, imgSize);
		if (_showEvaluationBar) {

			drawEvaluationBar();
		}
		drawPlayerTime(chess::Color::White);
		drawPlayerTime(chess::Color::Black);
		if (_calcBoardPlacement)
			_imgBoard.draw(imgRect, gui::Image::AspectRatio::Keep, td::HAlignment::Center, td::VAlignment::Center, &_boardPlacement);
		else
			_imgBoard.draw(imgRect);

		gui::CoordType origX = _boardPlacement.left + _originXMargin * _boardPlacement.width();
		gui::CoordType origY = _boardPlacement.bottom - _originYMargin * _boardPlacement.height();

		gui::CoordType matrixRight = _boardPlacement.right - _originXMargin * _boardPlacement.width();
		gui::CoordType matrixTop = _boardPlacement.top + _originYMargin * _boardPlacement.height();

		gui::Rect rMatrix(origX, matrixTop, matrixRight, origY);
		gui::Shape::drawRect(rMatrix, td::ColorID::White, 2.5f);  // use white

		gui::CoordType cellWidth = (matrixRight - origX) / N;
		gui::CoordType cellHeight = (origY - matrixTop) / N;


		_ptOrig = { origX, origY - cellHeight };
		_cellSize = { cellWidth, cellHeight };
		//draw pieces
		if (reDrawBoard) {

			chessBoard = _board.getBoard();
			reDrawBoard = false;
		}
		auto legalMoves = _board.getAllLegalMoves();
		for (auto rows : chessBoard) {
			for (auto sq : rows) {
				auto sqPos = _playerColor == chess::Color::White ? chess::Position::flipPosition(sq.getPosition()) : chess::Position::flipHorizontaly(sq.getPosition());
				gui::Rect rect;
				getCellRect(rect, sqPos);
				if ((_userMove == UserMove::DragBK) && sq.getPosition() == getCellCoordinate(_dragStartPoint))
				{
					auto pt = _currentDragPoint - _dragStartPoint;
					rect += pt;
				}
				if (sq.hasPiece()) {
					pieceImages[sq.getPiece().getPieceEncoding()]->draw(rect);
				}
				if ((_userMove == UserMove::DragBK || _userMove == UserMove::ClickedBK)) {
					for (auto mv : legalMoves.moves) {
						if (mv.getFromPos() == getCellCoordinate(_dragStartPoint)) {
							auto toPos = _playerColor == chess::Color::White ? chess::Position::flipPosition(mv.getToPos()) : chess::Position::flipHorizontaly(mv.getToPos());

							gui::Circle c;
							getCellCircle(c, toPos);
							c.r = 12;
							gui::Shape _possibleMovesSelection;
							_possibleMovesSelection.createCircle(c, 4.0f);
							_possibleMovesSelection.drawFill(td::ColorID::Gray);

						}
					}

				}

			}

		}
	}

	std::thread& getThread()
	{
		return _workingThread;
	}



	void updateImgSizes() {
		int margin = 120;
		imgSize.width = _viewSize.height - margin;
		imgSize.height = _viewSize.height - margin;

		imgPt.x = (_viewSize.width / 2) - imgSize.width / 2 - 100;
		imgPt.y = margin / 2;
	}
	void updateModelSize(const gui::Size& newSize)
	{
		_calcBoardPlacement = true;
		_viewSize = newSize;
		updateImgSizes();
	}
	bool canPlayerMakeMove(const gui::Point&) const
	{
		return (_board.sideToMove == _playerColor);
	}
	bool isButtonClicked(const gui::Point& cursorPoint)
	{
		if (evaluationBarButton.left <= cursorPoint.x && cursorPoint.x <= evaluationBarButton.right &&
			evaluationBarButton.top <= cursorPoint.y && cursorPoint.y <= evaluationBarButton.bottom) {
			_showEvaluationBar = !_showEvaluationBar;
			return true;
		}
		if (_newGameBtn.left <= cursorPoint.x && cursorPoint.x <= _newGameBtn.right &&
			_newGameBtn.top <= cursorPoint.y && cursorPoint.y <= _newGameBtn.bottom) {
			reset();
			return true;
		}
		return false;
	}
	bool canDragPiece(const gui::Point& cursorPoint)
	{

		if (_board.sideToMove != _playerColor) return false;


		auto cell = getCellCoordinate(cursorPoint);
		chess::Square sq = _board.getSquare(cell);

		if (sq.hasPiece())
		{
			if (sq.getPiece().getColor() == _playerColor) {

				_userMove = UserMove::ClickedBK;
				_dragStartPoint = cursorPoint;
				return true;
			}
		}
		_userMove = UserMove::None;
		return false;
	}

	bool finishUserMove(const gui::Point& cursorPoint, chess::PieceType promotedPiece)
	{
		if (_userMove == UserMove::DragBK)
		{
			auto endCell = getCellCoordinate(cursorPoint);
			auto startCell = getCellCoordinate(_dragStartPoint);
			auto legalMoves = _board.getAllLegalMoves();
			auto c = legalMoves.count;
			auto m = _board;
			bool enPassant = false;
			chess::Castling castling = chess::Castling::NONE;
			chess::Square startSq = _board.getSquare(startCell);
			if (startSq.hasPiece() && startSq.getPiece().getPieceType() == chess::PieceType::Pawn) {
				if (endCell.column != startCell.column && !(_board.getSquare(endCell).hasPiece())) {
					enPassant = true;
				}
				if (endCell.row == 0 || endCell.row == 7) {
					if (promotedPiece == chess::PieceType::None) {
						_promotionCallback(_playerColor, const_cast<gui::Point&>(cursorPoint));
						return false;
					}
				}
			}
			if (startSq.hasPiece() && startSq.getPiece().getPieceType() == chess::PieceType::King) {
				if (endCell.row == startCell.row && std::abs(startCell.column - endCell.column) > 1) {
					castling = (endCell.column > startCell.column) ? chess::Castling::King : chess::Castling::Queen;

				}
			}
			chess::Move move(startCell, endCell, enPassant, castling, promotedPiece);
			if (legalMoves.has(move)) {
				addMoveToHistory(move);
				_board.makeMove(move);
				checkGameResult();
				timerTick(!_playerColor);

				reDrawBoard = true;
				evalScore = _board.evaluate();
				//_playerColor = !_playerColor;
				_userMove = UserMove::None;
				return true;
			}
		}
		_userMove = UserMove::None;
		return false;
	}

	bool testDrag(const gui::Point& cursorPoint)
	{
		if (_userMove == UserMove::ClickedBK)
		{
			_currentDragPoint = cursorPoint;
			_userMove = UserMove::DragBK;
			return true;
		}
		else if (_userMove == UserMove::DragBK)
		{
			_currentDragPoint = cursorPoint;
			return true;
		}
		_userMove = UserMove::None;
		return false;
	}



	bool canAIMakeMove() const
	{
		//return false;
		return _board.sideToMove == !_playerColor;
	}
	int getAddaptiveDepth()
	{
		auto k = _board.estimatePositions(5);
 if (k < 150000) {

			return 7;
		}
		else if (k < 500000) {

			return 6;
		}
		else {

			return 5;
		}
	}
	int getDepthByDifficulty()
	{
		switch (_chessDifficulty)
		{
		case ChessDifficulty::Easy:
			return 2;
		case ChessDifficulty::Medium:
			return 4;
		case ChessDifficulty::Hard:
			return getAddaptiveDepth();
		default:
			return 4;
		}
	}
	void findBestMove(const gui::thread::MainThreadSharedFunction1& callBack)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(300)); // 300ms delay
		auto move = _board.findBestMove(getDepthByDifficulty());
		evalScore = move.second;
		addMoveToHistory(move.first);
		_board.makeMove(move.first);
		checkGameResult();
		reDrawBoard = true;
		timerTick(_playerColor);

		td::INT4 iMsg = td::INT4(1);
		td::Variant varMsg(iMsg);
		gui::thread::asyncExecInMainThread(callBack, varMsg);

		//	mu::dbgLog("Best evaluation: %d", bestScore);
	}
	void moveAI(gui::thread::MainThreadSharedFunction1 callBack)
	{
		//for each new AI move, fire new thread
		_workingThread = std::thread(&ChessBackend::findBestMove, this, callBack);
	}

};



