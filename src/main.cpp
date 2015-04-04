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

#include "mainwindow.h"
#include "stwindow.h"
#include <QApplication>
#include <QObject>
#include <iostream>

using namespace std::placeholders;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Controller *controller = Controller::instance();
    Agent *agent = new Agent(controller);

    /*
    MainWindow w(controller,agent);
    std::function<void()> fpShowGUI = bind(&MainWindow::showGUI, &w);
    std::function<void()> fpHideGUI = bind(&MainWindow::hideGUI, &w);
    controller->setGUIFunctions(fpHideGUI, fpShowGUI);
    */

    STWindow w(controller,agent);
    QObject::connect(&a, SIGNAL(applicationStateChanged(Qt::ApplicationState)), &w, SLOT(checkState(Qt::ApplicationState)));
    std::function<void()> fpShowGUI = bind(&STWindow::showGUI, &w);
    std::function<void()> fpHideGUI = bind(&STWindow::hideGUI, &w);
    controller->setGUIFunctions(fpHideGUI, fpShowGUI);

    // send handleOutput() to controller so that it can say things directly.
    std::function<void(std::vector<std::string> args)> fpSayGUI = bind(&STWindow::handleOutput, &w, _1);
    controller->fpSayGUI = fpSayGUI;

    //w.show();

    return a.exec();
}
