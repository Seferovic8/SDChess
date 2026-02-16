#pragma once
#include <gui/Canvas.h>
#include <gui/Sound.h>
//#include "QueenModel.h"
#include <gui/Canvas.h>
#include <gui/Shape.h>
#include <gui/Button.h>
#include <gui/DrawableString.h>
#include <gui/Transformation.h>
#include <gui/Font.h>
//
//  Created by Izudin Dzafic on 18/10/2025.
//  Copyright © 2025 IDz. All rights reserved.
//
#pragma once
#include <gui/Window.h>
//#include "ViewChessBoard.h"     //This is the main view
#include <functional>
//#include "ViewSettings.h"
#include <gui/Panel.h>
#include <gui/VerticalLayout.h>
#include "SelectionPage.h"

#include <gui/Label.h>
#include <gui/LineEdit.h>
#include <gui/TextEdit.h>

//
//  Created by Izudin Dzafic on 18/10/2025.
//  Copyright © 2025 IDz. All rights reserved.
//
#pragma once
#include <gui/Window.h>
#include "ChessWindow.h"
//#include "ViewChessBoard.h"     //This is the main view
#include <functional>
//#include "ViewSettings.h"
//#include "ViewSettings.h"
#include <gui/Panel.h>
#include "SelectionPage.h"

//
//  Created by Izudin Dzafic on 28/07/2020.
//  Copyright © 2020 IDz. All rights reserved.
//
#pragma once
#include <gui/View.h>
#include <gui/VerticalLayout.h>
#include <gui/Label.h>
#include <gui/DrawableString.h>
#include <gui/LineEdit.h>
#include <gui/TextEdit.h>
#include <gui/Button.h>

struct RectSizes {
	int x1;
	int y1;
	int x2;
	int y2;
	void setSizes(int _x1, int _y1, int _x2, int _y2) {
		x1 = _x1;
		y1 = _y1;
		x2 = _x2;
		y2 = _y2;
	}
	RectSizes() {}
	RectSizes(int _x1, int _y1, int _x2, int _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
};
class SelectionPage : public gui::Canvas {
public:
	std::function<void()> closeInitialWin;

protected:
	gui::Shape _modal;
	gui::Shape _regularSelection;
	gui::Shape _c960Selection;
	gui::Shape _playSelection;
	gui::Shape _colorSelection;
	gui::Shape _diffiuciltySelection;
	gui::Font _fontWelcome;
	gui::Font _fontSelect;
	gui::Font _fontRegular;
	gui::Font _fontExplanation;
	gui::Font _fontDifficulty;
	gui::Font _fontButton;
	gui::DrawableString _strWelcome = "Welcome";
	gui::DrawableString _strSelect = "Select your game mode to begin";
	gui::DrawableString _strGameSelectionRegular = "Regular Chess";
	gui::DrawableString _strGameSelection960 = "Chess 960";
	gui::DrawableString _strGameSelectionExplanationRegular = "Classic game";
	gui::DrawableString _strGameSelectionExplanation960 = "A variant with randomized starting positions";
	gui::DrawableString _strGameDifficultyEasy = "Easy";
	gui::DrawableString _strGameDifficultyMedium = "Medium";
	gui::DrawableString _strGameDifficultyHard = "Hard";
	gui::DrawableString _strPlay = "Play";
	gui::DrawableString _strColorWhite = "White";
	gui::DrawableString _strColorBlack = "Black";
	gui::DrawableString _strColorIdk = "?";
	ChessType chessType = ChessType::Regular;
	ChessDifficulty chessDifficulty = ChessDifficulty::Medium;
	ChessColor chessColor = ChessColor::Random;
	RectSizes regularRectSizes;
	RectSizes chess960RectSizes;
	RectSizes easyRectSizes;
	RectSizes mediumRectSizes;
	RectSizes hardRectSizes;
	RectSizes playRectSizes;
	RectSizes colorWhiteRectSizes;
	RectSizes colorBlackRectSizes;
	RectSizes colorRandRectSizes;
	void onDraw(const gui::Rect& rectangleCanvas) override {
		gui::Transformation tr;
		tr.saveContext();



		// tr.scale(1.3);
		tr.translate(gui::Point(0, 0));
		tr.appendToContext();

		int zoneWidth = 250;
		int zoneHeight = 300;
		int zoneTop = 200;
		int margin = 100;
		int x = rectangleCanvas.width() - rectangleCanvas.width() / 2;
		int y = rectangleCanvas.height() - rectangleCanvas.height() / 2;
		gui::Rect modalRect(x - zoneWidth, y - zoneHeight, x + zoneWidth, y + zoneHeight);
		_modal.createRoundedRect(modalRect,15, 2.0f, td::LinePattern::Solid);
		_modal.drawFill(td::ColorID::SlateGray);
		{
			int rectWidth = 2 * zoneWidth;
			int rectHeight = 2 * zoneHeight;
			modalRect.translate(0, 20);
			//tr.appendToContext();
			_strWelcome.draw(modalRect, &_fontWelcome, td::ColorID::White, td::TextAlignment::Center);
			modalRect.translate(0, 60);

			_strSelect.draw(modalRect, &_fontSelect, td::ColorID::LightGray, td::TextAlignment::Center);
		}
		{
			gui::Rect regularRect(regularRectSizes.x1, regularRectSizes.y1, regularRectSizes.x2, regularRectSizes.y2);
			_regularSelection.createRoundedRect(regularRect, 15, 3.0f);
			_regularSelection.drawFillAndWire(td::ColorID::SysBackAlt2, td::ColorID::Green);
			regularRect.translate(15, 20);
			_strGameSelectionRegular.draw(regularRect, &_fontRegular, td::ColorID::LightGray, td::TextAlignment::Left);
			regularRect.translate(0, 35);
			_strGameSelectionExplanationRegular.draw(regularRect, &_fontExplanation, td::ColorID::LightGray, td::TextAlignment::Left);
			gui::Circle c(regularRectSizes.x2 + 10 - 50, (regularRectSizes.y1 + 50), 10);
			_regularSelection.createCircle(c, 4.0f);
			auto bla = chessType;
			_regularSelection.drawWire((chessType == ChessType::Regular) ? td::ColorID::Blue : td::ColorID::White);
		}
		{

			gui::Rect C960Rect(chess960RectSizes.x1, chess960RectSizes.y1, chess960RectSizes.x2, chess960RectSizes.y2);
			_c960Selection.createRoundedRect(C960Rect, 15, 3.0f);
			_c960Selection.drawFillAndWire(td::ColorID::SysBackAlt2, td::ColorID::Green);
			C960Rect.translate(15, 20);
			_strGameSelection960.draw(C960Rect, &_fontRegular, td::ColorID::LightGray, td::TextAlignment::Left);
			C960Rect.translate(0, 35);
			_strGameSelectionExplanation960.draw(C960Rect, &_fontExplanation, td::ColorID::LightGray, td::TextAlignment::Left);
			gui::Circle c(chess960RectSizes.x2 + 10 - 50, (chess960RectSizes.y1 + 50), 10);
			_c960Selection.createCircle(c, 4.0f);
			_c960Selection.drawWire((chessType == ChessType::Chess960) ? td::ColorID::Blue : td::ColorID::White);
		}
		{
			auto selectedColor = td::ColorID::SysSelectedItemBack;
			//Easy
			gui::Rect easyRect(easyRectSizes.x1, easyRectSizes.y1, easyRectSizes.x2, easyRectSizes.y2);
			_diffiuciltySelection.createRoundedRect(easyRect, 15, 3.0f);
			_diffiuciltySelection.drawFillAndWire(chessDifficulty==ChessDifficulty::Easy?selectedColor: td::ColorID::SysBackAlt2, chessDifficulty == ChessDifficulty::Easy ?  td::ColorID::Green : td::ColorID::SysBackAlt2);
			_strGameDifficultyEasy.draw(easyRect, &_fontDifficulty, td::ColorID::LightGray, td::TextAlignment::Center, td::VAlignment::Center);

			//Medium
			gui::Rect mediumRect(mediumRectSizes.x1, mediumRectSizes.y1, mediumRectSizes.x2, mediumRectSizes.y2);
			_diffiuciltySelection.createRoundedRect(mediumRect, 15, 3.0f);
			_diffiuciltySelection.drawFillAndWire(chessDifficulty == ChessDifficulty::Medium ? selectedColor : td::ColorID::SysBackAlt2, chessDifficulty == ChessDifficulty::Medium ? td::ColorID::Green : td::ColorID::SysBackAlt2);
			_strGameDifficultyMedium.draw(mediumRect, &_fontDifficulty, td::ColorID::LightGray, td::TextAlignment::Center, td::VAlignment::Center);

			//Hard
			gui::Rect hardRect(hardRectSizes.x1, hardRectSizes.y1, hardRectSizes.x2, hardRectSizes.y2);
			_diffiuciltySelection.createRoundedRect(hardRect, 15, 3.0f);
			_diffiuciltySelection.drawFillAndWire(chessDifficulty == ChessDifficulty::Hard ? selectedColor : td::ColorID::SysBackAlt2, chessDifficulty == ChessDifficulty::Hard ? td::ColorID::Green : td::ColorID::SysBackAlt2);
			_strGameDifficultyHard.draw(hardRect, &_fontDifficulty, td::ColorID::LightGray, td::TextAlignment::Center, td::VAlignment::Center);
		}
		{
			//Play
			gui::Rect playRect(playRectSizes.x1, playRectSizes.y1, playRectSizes.x2, playRectSizes.y2);
			_playSelection.createRoundedRect(playRect, 15, 3.0f);
			_playSelection.drawFillAndWire(td::ColorID::Green, td::ColorID::Green);
			_strPlay.draw(playRect, &_fontDifficulty, td::ColorID::LightGray, td::TextAlignment::Center, td::VAlignment::Center);
		}
		{
			
			//White Picker
			gui::Rect whiteRect(colorWhiteRectSizes.x1, colorWhiteRectSizes.y1, colorWhiteRectSizes.x2, colorWhiteRectSizes.y2);
			_colorSelection.createRoundedRect(whiteRect, 15, 4.0f);
			_colorSelection.drawFillAndWire(chessColor == ChessColor::White ? td::ColorID::White : td::ColorID::LightGray, 
				chessColor == ChessColor::White ? td::ColorID::Green : td::ColorID::LightGray);
			_strColorWhite.draw(whiteRect, &_fontDifficulty, td::ColorID::Black, td::TextAlignment::Center, td::VAlignment::Center);
			
			//Rand
			gui::Rect randRect(colorRandRectSizes.x1, colorRandRectSizes.y1, colorRandRectSizes.x2, colorRandRectSizes.y2);
			_colorSelection.createRoundedRect(randRect, 15, 4.0f);
			_colorSelection.drawFillAndWire(chessColor == ChessColor::Random ? td::ColorID::LightCoral : td::ColorID::Coral, 
				chessColor == ChessColor::Random ? td::ColorID::Green : td::ColorID::Coral);
			_strColorIdk.draw(randRect, &_fontDifficulty, td::ColorID::Black, td::TextAlignment::Center, td::VAlignment::Center);

			//Black
			gui::Rect blackRect(colorBlackRectSizes.x1, colorBlackRectSizes.y1, colorBlackRectSizes.x2, colorBlackRectSizes.y2);
			_colorSelection.createRoundedRect(blackRect, 15, 4.0f);
			_colorSelection.drawFillAndWire(chessColor == ChessColor::Black ? td::ColorID::Black : td::ColorID::SysBackAlt1, 
				chessColor == ChessColor::Black ? td::ColorID::Green : td::ColorID::SysBackAlt1);
			_strColorBlack.draw(blackRect, &_fontDifficulty, td::ColorID::LightGray, td::TextAlignment::Center, td::VAlignment::Center);
		}
	}
	void onResize(const gui::Size& newSize) override
	{
		int zoneWidth = 250;
		int zoneHeight = 300;
		int zoneTop = 200;
		int margin = 100;
		int x = newSize.width - newSize.width / 2;
		int y = newSize.height - newSize.height / 2;
		int rectX1 = x - zoneWidth;
		int rectX2 = x + zoneWidth;
		int rectY1 = y - zoneHeight + 150;
		int rectY2 = 2 * zoneHeight;
		//regular rectangle
		regularRectSizes.setSizes(rectX1 + 10, rectY1, rectX2 - 10, rectY1 + 100);

		//960 rectangle
		rectY1 = y - zoneHeight + 260;
		chess960RectSizes.setSizes(rectX1 + 10, rectY1, rectX2 - 10, rectY1 + 100);

		//easy rectangle
		rectY1 = y - zoneHeight + 370;
		int rectWidth = (2 * zoneWidth - 40) / 3;

		//Easy
		int currentX1 = rectX1 + 10;
		int currentX2 = currentX1 + rectWidth;
		easyRectSizes.setSizes(currentX1, rectY1, currentX2, rectY1 + 60);

		// Medium
		currentX1 = currentX2 + 10;
		currentX2 = currentX1 + rectWidth;
		mediumRectSizes.setSizes(currentX1, rectY1, currentX2, rectY1 + 60);

		// Hard
		currentX1 = currentX2 + 10;
		currentX2 = currentX1 + rectWidth;
		hardRectSizes.setSizes(currentX1, rectY1, currentX2, rectY1 + 60);

		// Play
		rectY1 = y + zoneWidth - 30;
		playRectSizes.setSizes(rectX1 + 10, rectY1, rectX2 - 10, rectY1 + 60);
		
		// Color picker
		//WHite
		rectY1 = y - zoneHeight + 445;
		currentX1 = rectX1 + 10;
		rectWidth = rectWidth + 40;
		currentX2 = currentX1 + rectWidth;
		colorWhiteRectSizes.setSizes(currentX1, rectY1, currentX2, rectY1 + 60);
		// Rand
		int randWdth = (rectX2 - rectX1) - 2 * (currentX2 - currentX1) - 40;
		currentX1 = currentX2 + 10;
		currentX2 = currentX1 + randWdth;
		colorRandRectSizes.setSizes(currentX1, rectY1, currentX2, rectY1 + 60);

		// Black
		currentX1 = currentX2 + 10;
		currentX2 = currentX1 + rectWidth;
		colorBlackRectSizes.setSizes(currentX1, rectY1, currentX2, rectY1 + 60);


	}
	void onPrimaryButtonPressed(const gui::InputDevice& inputDevice) override
	{
		gui::Point click = inputDevice.getFramePoint();
		if ((click.x >= regularRectSizes.x1 && click.x <= regularRectSizes.x2) &&
			(click.y >= regularRectSizes.y1 && click.y <= regularRectSizes.y2)) {
			chessType = ChessType::Regular;

		}
		if ((click.x >= chess960RectSizes.x1 && click.x <= chess960RectSizes.x2) &&
			(click.y >= chess960RectSizes.y1 && click.y <= chess960RectSizes.y2)) {
			chessType = ChessType::Chess960;

		}

		// Difficulty
		if ((click.x >= easyRectSizes.x1 && click.x <= easyRectSizes.x2) &&
			(click.y >= easyRectSizes.y1 && click.y <= easyRectSizes.y2)) {
			chessDifficulty = ChessDifficulty::Easy;

		}
		if ((click.x >= mediumRectSizes.x1 && click.x <= mediumRectSizes.x2) &&
			(click.y >= mediumRectSizes.y1 && click.y <= mediumRectSizes.y2)) {
			chessDifficulty = ChessDifficulty::Medium;

		}
		if ((click.x >= hardRectSizes.x1 && click.x <= hardRectSizes.x2) &&
			(click.y >= hardRectSizes.y1 && click.y <= hardRectSizes.y2)) {
			chessDifficulty = ChessDifficulty::Hard;
		}

		// Color
		if ((click.x >= colorWhiteRectSizes.x1 && click.x <= colorWhiteRectSizes.x2) &&
			(click.y >= colorWhiteRectSizes.y1 && click.y <= colorWhiteRectSizes.y2)) {
			chessColor = ChessColor::White;

		}
		if ((click.x >= colorRandRectSizes.x1 && click.x <= colorRandRectSizes.x2) &&
			(click.y >= colorRandRectSizes.y1 && click.y <= colorRandRectSizes.y2)) {
			chessColor = ChessColor::Random;

		}
		if ((click.x >= colorBlackRectSizes.x1 && click.x <= colorBlackRectSizes.x2) &&
			(click.y >= colorBlackRectSizes.y1 && click.y <= colorBlackRectSizes.y2)) {
			chessColor = ChessColor::Black;
		}

		// Play
		if ((click.x >= playRectSizes.x1 && click.x <= playRectSizes.x2) &&
			(click.y >= playRectSizes.y1 && click.y <= playRectSizes.y2)) {
		//	auto k = pWnd->getFixSizes();
			FixSizes f = FixSizes::FixAuto;

			ChessWindow* window = new ChessWindow(chessType, chessDifficulty, chessColor);
			setAsCentralViewInWindow(window,f);
			window->open();
			closeInitialWin();
			return;
		}
		reDraw();
	}

public:
	//	TestWindow s;
	SelectionPage() :gui::Canvas({ gui::InputDevice::Event::PrimaryClicks}) 
		 {
		enableResizeEvent(true);

		_fontWelcome.create("Roboto", 28.0f, gui::Font::Style::Bold, gui::Font::Unit::LogicalPixel);
		_fontSelect.create("Roboto", 14.0f, gui::Font::Style::Normal, gui::Font::Unit::LogicalPixel);
		_fontRegular.create("Roboto", 14.0f, gui::Font::Style::Bold, gui::Font::Unit::LogicalPixel);
		_fontExplanation.create("Roboto", 12.0f, gui::Font::Style::Normal, gui::Font::Unit::LogicalPixel);
		_fontDifficulty.create("Inter", 12.0f, gui::Font::Style::Bold, gui::Font::Unit::LogicalPixel);
		_fontButton.create("Inter", 16.0f, gui::Font::Style::Bold, gui::Font::Unit::LogicalPixel);
		setBackgroundColor(td::ColorID::SysBackAlt2);
		reDraw();
		//resetSimulation();
	}
};