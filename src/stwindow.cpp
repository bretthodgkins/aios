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

#include "stwindow.h"
#include "ui_stwindow.h"

STWindow::STWindow(Controller *c, Agent *a, QWidget *parent) :
        QWidget(parent),
        c(c),
        a(a),
        ui(new Ui::STWindow)
    {
    ui->setupUi(this);

    quickCommandAction = new QAction(tr("Quick &Command..."), this);
    connect(quickCommandAction, SIGNAL(triggered()), this, SLOT(showGUI()));
    presentationModeAction = new QAction(tr("Enter &Presentation Mode..."), this);
    connect(presentationModeAction, SIGNAL(triggered()), this, SLOT(presentationMode()));
    quitAction = new QAction(tr("&Quit AIOS"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(quickCommandAction);
    trayIconMenu->addAction(presentationModeAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon("../Resources/graphics/aios3.tif"));
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();

    setupDisplay();

    // this allows a customisable load in script
    handleOutput(a->handleInput("onLoad"));
}

STWindow::~STWindow()
{
    delete ui;
}

void STWindow::setupDisplay()
{
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    //this->setStyleSheet("* { background-color: black; }");
    //this->setWindowOpacity(0.6);
    ui->txtCommand->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->txtCommand->setText(QString::fromStdString(""));
}

void STWindow::showGUI()
{
    showNormal();
    raise();
    activateWindow();
}

void STWindow::hideGUI()
{
    hide();
}

void STWindow::presentationMode()
{
    std::cout << "pres\n";
}

void STWindow::checkState(Qt::ApplicationState state)
{
    if (state != Qt::ApplicationActive)
        hideGUI();
}

void STWindow::handleOutput(std::vector<string> output)
{
    if (output.size()>0 && output[0] != "")
    {
        ui->txtCommand->setPlaceholderText(QString::fromStdString(output[0]));

        // runs the text to speech function on another thread
        QtConcurrent::run(&(*c->nc), &NativeController::say, output[0]);
    }
}

void STWindow::on_txtCommand_returnPressed()
{
    handleOutput(a->handleInput(ui->txtCommand->text().toUtf8().constData()));
    ui->txtCommand->setText("");
}
