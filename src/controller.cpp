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

#include "controller.h"

Controller *Controller::s_instance = 0;

void Controller::setGUIFunctions(std::function<void()> argFpHideGUI, std::function<void()> argFpShowGUI)
{
    this->fpHideGUI = argFpHideGUI;
    this->fpShowGUI = argFpShowGUI;
    usingGUI = true;
}

bool Controller::clickOnImage(vector<string> args)
{
    double confidence;
    pair <int,int> location;

    // this method must have exactly one argument
    if (args.size() == 1)
    {
        if (!findImageOnScreen(args[0], location, confidence))
            return false;

        if (confidence < 0.6)
        {
            nc->moveCursor(location.first,location.second);
            say({"I'm not sure. Is this "+args[0]});
            return false;
        }
        nc->click(location.first,location.second);
        return true;
    }
    return false;

}

bool Controller::hover(vector<string> args)
{
    double confidence;
    pair <int,int> location;

    if (args.size() == 1)
    {
        if (!findImageOnScreen(args[0], location, confidence))
            return false;

        cout << confidence << endl;
        if (confidence < 0.6)
        {
            nc->moveCursor(location.first,location.second);
            say({"I'm not sure. Is this "+args[0]});
            return false;
        }
        nc->moveCursor(location.first,location.second);
        return true;
    }
    return false;
}

bool Controller::say(std::vector<string> args)
{
    if (args.size() > 0 && args[0] != "")
    {
        cout << args[0] << endl;
        // if we're using GUI, then let it handle this
        // this ensures it is displayed on GUI and also
        // on a seperate thread
        if (fpSayGUI)
        {
            fpSayGUI(args);
        } else {
            nc->say(args[0]);
        }
        return true;
    }
    return false;
}

bool Controller::showGUI(std::vector<string> args)
{
    (void)args; // this function does not use args
    if (usingGUI)
    {
        fpShowGUI();
    }
    return true;
}

bool Controller::hideGUI(std::vector<string> args)
{
    (void)args; // this function does not use args
    if (usingGUI)
    {
        fpHideGUI();
    }
    return true;
}

bool Controller::pressButton(vector<string> args)
{
    if (args.size() == 1)
    {
        nc->pressButton(args[0]);
        return true;
    }

    return false;
}

bool Controller::sendKeys(vector<string> args)
{
    if (args.size() == 1)
    {
        Util::fixEscapeCharacters(args[0]);
        nc->sendKeys(args[0]);
        return true;
    }

    return false;
}

bool Controller::systemCommand(vector<string> args)
{
    if (args.size() == 1)
    {
        return (nc->systemCommand(args[0]) == 0); // returns true if no error
    }

    return false;
}

bool Controller::wait(vector<string> args)
{
    if (args.size() == 1)
    {
        long int num;
        if (Util::stringToLongInt(args[0], num) && num > 0)
        {
            sleep(num);
            return true;
        }
    }
    return false;
}

bool Controller::waitUntilVisible(std::vector<string> args)
{
    double confidence;
    pair <int,int> location;

    if (!findImageOnScreen(args[0], location, confidence))
        return false;

    while (confidence < 0.6)
        findImageOnScreen(args[0], location, confidence);

    return true;
}

bool Controller::followWhileVisible(std::vector<string> args)
{
    double confidence;
    pair <int,int> location;
    if(!findImageOnScreen(args[0], location, confidence))
        return false;

    while (confidence > 0.6)
    {
        if(!reFindImageOnScreen(args[0], location, location, confidence))
            return false;
        if (confidence < 0.6)
            findImageOnScreen(args[0], location, confidence);
        nc->moveCursor(location.first,location.second);
    }

    return true;
}

bool Controller::findImageOnScreen(string imageName, pair <int,int> 
                                             &location, double &confidence)
{
    string filename = "../Resources/img/" + imageName + ".jpg";
    if (!Util::doesFileExist(filename))
    {
        string dialogue = "Can you please show me what "+imageName+" looks like?";
        say({dialogue});
        usleep(300);
        //sleep(1);
        if(!nc->capturePortionOfScreen(imageName))
            return false;
    }

    location = findImageUsingTemplateMatching(filename, nc->getScreenCapture(),
                    nc->getDisplayWidth(), nc->getDisplayHeight(), confidence);
    return true;
}

bool Controller::reFindImageOnScreen(string imageName, pair <int,int> prevLoc,
                                   pair <int,int> &location, double &confidence)
{
    string filename = "../Resources/img/" + imageName + ".jpg";
    if (!Util::doesFileExist(filename))
    {
        string dialogue = "Can you please show me what "+imageName+" looks like?";
        say({dialogue});
        if(!nc->capturePortionOfScreen(imageName))
            return false;
    }
    location = reFindImageUsingTemplateMatching(filename, nc->getScreenCapture(),
                    nc->getDisplayWidth(), nc->getDisplayHeight(), prevLoc, confidence);
    return true;
}

