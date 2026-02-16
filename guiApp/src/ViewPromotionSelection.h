//
//  Created by Izudin Dzafic on 18/10/2025.
//  Copyright © 2025 IDz. All rights reserved.
//
#pragma once
#include <gui/PopupView.h>
#include <gui/Label.h>
#include <gui/ComboBox.h>
#include <gui/CheckBox.h>
#include <gui/LineEdit.h>
#include <gui/Slider.h>
#include <gui/HorizontalLayout.h>
#include <gui/GridLayout.h>
#include <gui/GridComposer.h>
#include <gui/ToolBar.h>


class ViewPromotinSelection : public gui::View
{
protected:

	gui::Image _queen;
	gui::Image _rook;
	gui::Image _bishop;
	gui::Image _knight;

	gui::HorizontalLayout _hl;
	gui::Button _btnQueen;
	gui::Button _btnRook;
	gui::Button _btnBishop;
	gui::Button _btnKnight;

	using CloseDialogCallback =std::function<void(chess::PieceType pressedPiece)>;
	CloseDialogCallback _closeDialogCallback;
public:
	ViewPromotinSelection(chess::Color playerColor, CloseDialogCallback closeDialogCallback)
		: _queen(playerColor==chess::Color::White ? "../assets/wq.png": "../assets/bq.png")
		, _rook(playerColor==chess::Color::White ? "../assets/wr.png": "../assets/br.png")
		, _bishop(playerColor==chess::Color::White ? "../assets/wb.png": "../assets/bb.png")
		, _knight(playerColor==chess::Color::White ? "../assets/wn.png": "../assets/bn.png")
		, _btnQueen(&_queen,"")
		, _btnRook(&_rook,"")
		, _btnBishop(&_bishop,"")
		, _btnKnight(&_knight,"")
		, _closeDialogCallback(std::move(closeDialogCallback))
		, _hl(4)

	{
		gui::Application* pApp = getApplication();
		_btnQueen.onClick([this]() {
			_closeDialogCallback(chess::PieceType::Queen);
			});
		_btnQueen.setSize(gui::Size(148, 148));

		_btnRook.onClick([this]() {
			_closeDialogCallback(chess::PieceType::Rook);
			});
		_btnRook.setSize(gui::Size(148, 148));


		_btnBishop.onClick([this]() {
			_closeDialogCallback(chess::PieceType::Bishop);
			});
		_btnBishop.setSize(gui::Size(148, 148));

		_btnKnight.onClick([this]() {
			_closeDialogCallback(chess::PieceType::Knight);

			});
		_btnKnight.setSize(gui::Size(148, 148));
		_hl.append(_btnQueen);
		_hl.append(_btnRook);
		_hl.append(_btnBishop);
		_hl.append(_btnKnight);
		setLayout(&_hl);
	}


};
