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

#pragma once
#include <string>
#include <utility> // std::pair, std::make_pairs

class NativeController
{
public:
    virtual void click(unsigned xpos, unsigned ypos, int clickCount = 1) = 0;
    virtual void moveCursor(unsigned xpos, unsigned ypos) = 0;
    virtual void say(std::string text) = 0;
    virtual bool capturePortionOfScreen(std::string identifier) = 0;
    virtual void pressButton(std::string text) = 0;
    virtual void sendKeys(std::string text) = 0;
    virtual int systemCommand(std::string text) = 0;

    virtual unsigned char* getScreenCapture() = 0;
    virtual int getDisplayHeight() = 0;
    virtual int getDisplayWidth() = 0;
};

