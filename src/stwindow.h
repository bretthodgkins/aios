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

#ifndef STWINDOW_H
#define STWINDOW_H

#pragma once
#include <QWidget>
#include <QtCore>
#include <QSystemTrayIcon>
#include <QMenuBar>
#include <QString>
#include <QThread>
#include <QString>
#include <QtConcurrent/qtconcurrentrun.h>
#include <iostream>
#include "maccontroller.h"
#include "agent.h"

namespace Ui {
class STWindow;
}

class STWindow : public QWidget
{
    Q_OBJECT

public:
    Controller *c;
    Agent *a;

    explicit STWindow(Controller *c, Agent *a, QWidget *parent = 0);
    ~STWindow();

    void handleOutput(std::vector<std::string> args);

public slots:
    // if the state changes, and it's no longer active, it will call hideGUI()
    void checkState(Qt::ApplicationState state = Qt::ApplicationInactive);
    void showGUI();
    void hideGUI();
    void presentationMode();

private slots:
    void on_txtCommand_returnPressed();

private:
    Ui::STWindow *ui;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *quickCommandAction;
    QAction *presentationModeAction;
    QAction *quitAction;

    void setupDisplay();
};

#endif // STWINDOW_H
