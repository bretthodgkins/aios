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
#include <iostream>
#include <ApplicationServices/ApplicationServices.h>
#include <unistd.h>
#include <time.h>
#include <string>
#include <map>
#include <vector>
#include <utility> // std::pair, std::make_pairs
#include "nativecontroller.h"
#include "maccontroller.h"
#include "cv.h" // opencv functions
#include "util.h" // utility functions

class Controller
{
private:
    static Controller* s_instance;

    Controller()
    {
        nc = new MacController;
    }

public:
    static Controller* instance()
    {
        if (!s_instance)
            s_instance = new Controller;
        return s_instance;
    }

    NativeController *nc;

    // Allows controller to hide and show GUI if in use
    bool usingGUI = false;
    std::function<void()> fpHideGUI;
    std::function<void()> fpShowGUI;
    void setGUIFunctions(std::function<void()> fpHideGUI, std::function<void()> fpShowGUI);
    std::function<void(std::vector<std::string>)> fpSayGUI;
    
    // commands that can be called by primitive actions
    bool clickOnImage(std::vector<string> args);
    bool pressButton(std::vector<string> args);
    bool sendKeys(std::vector<string> args);
    bool systemCommand(std::vector<string> args);
    bool wait(std::vector<string> args);
    bool hover(std::vector<string> args);
    bool say(std::vector<string> args);
    bool showGUI(std::vector<string> args);
    bool hideGUI(std::vector<string> args);
    bool waitUntilVisible(std::vector<string> args);
    bool followWhileVisible(std::vector<string> args);

    bool findImageOnScreen(std::string imageName, pair <int,int>
                                          &location, double &confidence);
    bool reFindImageOnScreen(std::string imageName, pair <int,int> prevLoc,
                             pair <int,int> &location, double &confidence);
};
