//
//  Created by Izudin Dzafic on 18/10/2025.
//  Copyright © 2025 IDz. All rights reserved.
//
#pragma once
#include <gui/Canvas.h>
#include <gui/Sound.h>
#include <gui/ToolBar.h>
//#include "QueenModel.h"
#include "ChessBackend.h"
#include "ViewPromotionSelection.h"
#include "Dialog.h"
#include "chessLogic/Board/Board.h"

class ViewChessBoard : public gui::Canvas
{
protected:
	//ChessType chessType;
	//ChessDifficulty chessDifficulty;
	//ChessColor chessColor;
	ChessBackend _chessBackend;
	gui::Sound _soundMove;
	gui::Sound _soundMissionSuccess;
	gui::Size _size;
protected:

	void onResize(const gui::Size& newSize) override
	{
		_size = newSize;
		_chessBackend.updateModelSize(newSize);
	}

	void onDraw(const gui::Rect& rect) override
	{
		_chessBackend.draw();
		_chessBackend.drawMoves(rect);
		_chessBackend.drawButton(rect);
		if (_chessBackend.winnerPanelDraw) {

			_chessBackend.drawWinner(rect);
		
		}
	}
	void getPromotedPiece(chess::Color playerColor, const gui::Point& modelPoint) {
		gui::Window* pWnd = getParentWindow();
		assert(pWnd);

		auto statusBar = pWnd->getStatusBar();
		gui::Size sz(400, 300);

		// 🔴 parent je WINDOW, ne view
		DialogSettings* pSettingsDlg =
			new DialogSettings(
				pWnd,
				playerColor,
				[this, modelPoint](chess::PieceType pressedPiece)
				{
					playMove(modelPoint, pressedPiece);
				}
			);

		pSettingsDlg->setBackgroundColor(td::ColorID::White);
		// 1️⃣ OPEN
		pSettingsDlg->openModal();

		pSettingsDlg->keepOnTopOfParent();
	}
	void playMove(const gui::Point& modelPoint, chess::PieceType promotedPiece = chess::PieceType::None) {
		if (_chessBackend.finishUserMove(modelPoint, promotedPiece))
		{
			_soundMove.play();
			playAIMove();
			reDraw();
		}
	}
	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice) override
	{

		const gui::Point& modelPoint = inputDevice.getModelPoint();
		if (!_chessBackend.isButtonClicked(modelPoint))
		{
			_chessBackend.canDragPiece(modelPoint);
		}
	}
	void playAIMove()
	{
		if (_chessBackend.canAIMakeMove())
		{
			//start move AI thread
			auto mainThreadCallBack = std::make_shared<gui::thread::MainThreadFunction1>(std::bind(&ViewChessBoard::chessEngineCallBack, this, std::placeholders::_1));
			_chessBackend.moveAI(mainThreadCallBack);
		}
	}
	void onPrimaryButtonReleased(const gui::InputDevice& inputDevice) override
	{
		const gui::Point& modelPoint = inputDevice.getModelPoint();
		if (_chessBackend.canPlayerMakeMove(modelPoint)) {

			playMove(modelPoint);
			reDraw();
		}
	}

	void onCursorDragged(const gui::InputDevice& inputDevice) override //onMouseDragged(const gui::Point& modelPoint, td::UINT4 keyModifiers)
	{
		const gui::Point& modelPoint = inputDevice.getModelPoint();
		if (_chessBackend.testDrag(modelPoint))
			reDraw();
	}

	void chessEngineCallBack(td::Variant param)
	{
		td::INT4 iVal = param.i4Val();

		switch (iVal)
		{
		case 3:
			onWorkerCompleted();
			break;
		case 2:
			onWorkerCompleted();
			_soundMissionSuccess.play();
			break;
		case 1:
		{
			_soundMove.play();
			//clean the thread state
			auto& th = _chessBackend.getThread();
			if (th.joinable())
				th.join();
		}
		break;
		default:
			assert(false); //we have to handle all message types
		}
		reDraw(); //invoke reDraw manually after each state change
	}

public:
	ViewChessBoard(ChessType chessType, ChessDifficulty chessDifficulty, chess::Color chessColor)
		: Canvas({ gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::PrimaryClicks, gui::InputDevice::Event::CursorDrag })
		, _chessBackend(chessDifficulty, chessType==ChessType::Chess960, chessColor, [this](chess::Color color, const gui::Point& modelPoint)
			{
				this->getPromotedPiece(color, modelPoint);
			}, [this]()
				{
					this->reset();
				})
		, _soundMove(":move")
		, _soundMissionSuccess(":success")
	{
		setPreferredFrameRateRange(60, 60);
		enableResizeEvent(true); //required to obtain events whenever this view resizes (see onResize method in this class)
		startAnimation();
		if (chessColor == chess::Color::Black)
			playAIMove();
	}

	void onWorkerCompleted()
	{
		auto& th = _chessBackend.getThread();
		if (th.joinable())
			th.join();
		// _fnUpdateMenuAndTB();
		reDraw();
	}



	void refresh()
	{
		reDraw(); //this is async call and will be executed later
	}
	ViewChessBoard& getView()
	{
		return *this;
	}
	

	void reset()
	{
		getParentWindow()->close();
	}
};
