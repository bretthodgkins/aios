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
#include "ui_mainwindow.h"

MainWindow::MainWindow(Controller *c, Agent *a, QWidget *parent) :
    QMainWindow(parent),
    c(c),
    a(a),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupDisplay();

    // this allows a customisable load in script
    handleOutput(a->handleInput("onLoad"));
}

void MainWindow::hideGUI()
{
    showNormal();
    hide();
    //this->hide();
}

void MainWindow::showGUI()
{
    //this->activateWindow();
    //this->setFocus();
    showFullScreen();
    c->nc->click(100,100);
}

void MainWindow::setupDisplay()
{
    ui->txtCommand->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->txtCommand->setText(QString::fromStdString(""));

    // keep this window on top
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    QWidget::showFullScreen();
    /*
    raise();
    activateWindow();
    */
    qApp->processEvents(); // this allows us to use updated this->size() for background size
    //this->setWindowOpacity(1.0);

    QPixmap bkgnd("../Resources/graphics/forest.jpg");
    //bkgnd = bkgnd.scaled(this->size(), Qt::KeepAspectRatio);
    bkgnd = bkgnd.scaledToWidth(this->size().width(),Qt::SmoothTransformation);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    showGUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_txtCommand_returnPressed()
{
    handleOutput(a->handleInput(ui->txtCommand->text().toUtf8().constData()));
    ui->txtCommand->setText("");
}

void MainWindow::handleOutput(std::vector<string> output)
{
    if (output.size()>0 && output[0] != "")
    {
            ui->lblOutput->setText(QString::fromStdString(output[0]));

            // runs the text to speech function on another thread
            QtConcurrent::run(&(*c), &Controller::say, output);
    }
}
