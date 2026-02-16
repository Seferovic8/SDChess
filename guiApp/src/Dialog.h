#pragma once
#include <gui/Dialog.h>
#include "ViewPromotionSelection.h"
#include <gui/Application.h>

class DialogSettings : public gui::Dialog
{
protected:
    ViewPromotinSelection _viewPromotionSelection;
	chess::PieceType _pressedPiece = chess::PieceType::None;
    std::function<void(chess::PieceType)> _done;   // <- your callback

public:
    DialogSettings( gui::Frame* pFrame,chess::Color playerColor, std::function<void(chess::PieceType)> done)
        : gui::Dialog(pFrame, {}, gui::Size(450, 100)),
		_done(std::move(done))
        , _viewPromotionSelection(playerColor, [this](chess::PieceType pressedPiece)
            {
				_pressedPiece = pressedPiece;
                closeModalCallback(pressedPiece);
            })
    {
        setTitle(tr("dlgSettings"));
        setCentralView(&_viewPromotionSelection);
    }
    void closeModalCallback(chess::PieceType btn) {
        _done(btn);
        closeModal(gui::Dialog::Button::ID::OK);

    }
    ViewPromotinSelection& getView()
    {
        return _viewPromotionSelection;

    }
    chess::PieceType getPressedPiece() const
    {
        return _pressedPiece;

    }
};

