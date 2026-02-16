//
//  Created by Izudin Dzafic on 18/10/2025.
//  Copyright © 2025 IDz. All rights reserved.
//
#pragma once
#include <gui/Window.h>
//#include "MenuBar.h"
#include "SelectionPage.h"
//#include "ViewChessBoard.h"     //This is the main view
#include <functional>
//#include "ViewSettings.h"
//#include "ToolBar.h"
#include <gui/Panel.h>

class MainWindow : public gui::Window
{
protected:
    gui::Image _imgStart;
    gui::Image _imgStop;
    SelectionPage _mainView;
protected:
    
    void onInitialAppearance() override //will be called only once
    {
        _mainView.setFocus();
    }
    
public:
    MainWindow()
    : gui::Window(gui::Size(800, 800))
    , _imgStart(":start")
    , _imgStop(":stop")
    , _mainView()
  //  , _mainView(std::bind(&MainWindow::updateMenuAndTB, this))
    {
        setTitle(tr("appTitle"));
        setCentralView(&_mainView);
       // setCentralView(&_viewChessBoard);
  
        _mainView.closeInitialWin = [this]() {
            // ... switch the central view to the ChessBoard
            this->freeze();
            //this->close();
            };
    }
};
