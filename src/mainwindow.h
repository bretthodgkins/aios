/******************************************************************************
 * AIOS: A personal assistant that can learn anything you want
 * Copyright (C) 2015 Brett Hodgkins
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

//#ifndef MAINWINDOW_H
//#define MAINWINDOW_H
#pragma once
#include <QMainWindow>
#include <QtCore>
#include <iostream>
#include <QString>
#include <QThread>
#include <QString>
#include <QtConcurrent/qtconcurrentrun.h>
#include "maccontroller.h"
#include "agent.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Controller *c;
    Agent *a;

    explicit MainWindow(Controller *c, Agent *a, QWidget *parent = 0);
    ~MainWindow();

    void hideGUI();
    void showGUI();

private slots:
    void on_txtCommand_returnPressed();

private:
    Ui::MainWindow *ui;
    void setupDisplay();
    void handleOutput(std::vector<string> args);
};

//#endif // MAINWINDOW_H
