
#pragma once
#include <gui/Window.h>
//#include "MenuBar.h"
#include "ViewChessBoard.h"     //This is the main view
#include "Dialog.h"     //This is the main view
#include <functional>
//#include "ViewSettings.h"
//#include "ToolBar.h"
#include <gui/Panel.h>
#include <random>

static chess::Color resolveColor(ChessColor c)
{
    if (c == ChessColor::White)
    {
        return chess::Color::White;
    }
    else if (c == ChessColor::Black)
    {
        return chess::Color::Black;
    }
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(0, 1);

   return dist(rng) == 0 ? chess::Color::White : chess::Color::Black;
    
}
class ChessWindow : public gui::Window
{
protected:
    gui::Image _imgStart;
    gui::Image _imgStop;

    ViewChessBoard _viewChessBoard;
protected:
    
    void onInitialAppearance() override //will be called only once
    {
        _viewChessBoard.setFocus();
    }
    
public:
    ChessWindow(ChessType chessType,ChessDifficulty chessDifficulty, ChessColor chessColor)
    : gui::Window(gui::Size(1200, 800))
    , _imgStart(":start")
    , _imgStop(":stop")
        , _viewChessBoard(chessType, chessDifficulty, resolveColor(chessColor)) 

  //  , _mainView(std::bind(&MainWindow::updateMenuAndTB, this))
    {

        setTitle(tr("appTitle"));
        setCentralView(&_viewChessBoard);

    }
};
