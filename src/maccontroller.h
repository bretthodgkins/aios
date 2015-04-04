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
#include <unistd.h>
#include <time.h>
#include <string>
#include <map>
#include <utility> // std::pair, std::make_pairs
#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#include "nativecontroller.h"
#include "cv.h" // opencv functions

static int eventNumber = 150000;

class MacController: public NativeController
{
private:
    // map each character to it's CGKeyCode and whether the shift key is needed
    std::map<char,std::pair <int,bool>> charmap
    {
        {'a', {0x00,false}},
        {'s', {0x01,false}},
        {'d', {0x02,false}},
        {'f', {0x03,false}},
        {'h', {0x04,false}},
        {'g', {0x05,false}},
        {'z', {0x06,false}},
        {'x', {0x07,false}},
        {'c', {0x08,false}},
        {'v', {0x09,false}},
        {'b', {0x0B,false}},
        {'q', {0x0C,false}},
        {'w', {0x0D,false}},
        {'e', {0x0E,false}},
        {'r', {0x0F,false}},
        {'y', {0x10,false}},
        {'t', {0x11,false}},
        {'1', {0x12,false}},
        {'2', {0x13,false}},
        {'3', {0x14,false}},
        {'4', {0x15,false}},
        {'6', {0x16,false}},
        {'5', {0x17,false}},
        {'=', {0x18,false}},
        {'9', {0x19,false}},
        {'7', {0x1A,false}},
        {'-', {0x1B,false}},
        {'8', {0x1C,false}},
        {'0', {0x1D,false}},
        {']', {0x1E,false}},
        {'o', {0x1F,false}},
        {'u', {0x20,false}},
        {'[', {0x21,false}},
        {'i', {0x22,false}},
        {'p', {0x23,false}},
        {'l', {0x25,false}},
        {'j', {0x26,false}},
        {'\'', {0x27,false}},
        {'k', {0x28,false}},
        {';', {0x29,false}},
        {'\\', {0x2A,false}},
        {',', {0x2B,false}},
        {'/', {0x2C,false}},
        {'n', {0x2D,false}},
        {'m', {0x2E,false}},
        {'.', {0x2F,false}},
        {'`', {0x32,false}},
        {'A', {0x00,true}},
        {'S', {0x01,true}},
        {'D', {0x02,true}},
        {'F', {0x03,true}},
        {'H', {0x04,true}},
        {'G', {0x05,true}},
        {'Z', {0x06,true}},
        {'X', {0x07,true}},
        {'C', {0x08,true}},
        {'V', {0x09,true}},
        {'B', {0x0B,true}},
        {'Q', {0x0C,true}},
        {'W', {0x0D,true}},
        {'E', {0x0E,true}},
        {'R', {0x0F,true}},
        {'Y', {0x10,true}},
        {'T', {0x11,true}},
        {'!', {0x12,true}},
        {'@', {0x13,true}},
        {'#', {0x14,true}},
        {'$', {0x15,true}},
        {'^', {0x16,true}},
        {'%', {0x17,true}},
        {'+', {0x18,true}},
        {'(', {0x19,true}},
        {'&', {0x1A,true}},
        {'_', {0x1B,true}},
        {'*', {0x1C,true}},
        {')', {0x1D,true}},
        {'}', {0x1E,true}},
        {'O', {0x1F,true}},
        {'U', {0x20,true}},
        {'{', {0x21,true}},
        {'I', {0x22,true}},
        {'P', {0x23,true}},
        {'L', {0x25,true}},
        {'J', {0x26,true}},
        {'\"', {0x27,true}},
        {'K', {0x28,true}},
        {':', {0x29,true}},
        {'|', {0x2A,true}},
        {'<', {0x2B,true}},
        {'?', {0x2C,true}},
        {'N', {0x2D,true}},
        {'M', {0x2E,true}},
        {'>', {0x2F,true}},
        {'~', {0x32,true}},        
        {' ', {0x31,false}}, // Spacebar
        {'\n', {0x24,false}}, // Return
        {'\t', {0x30,false}} // Tab
    };
    std::map<std::string,int> buttonmap
    {
        {"up", 0x7E},
        {"down", 0x7D},
        {"left", 0x7B},
        {"right", 0x7C},
        {"tab", 0x30},
        {"caps lock", 0x39},
        {"caps", 0x39},
        {"shift", 0x38},
        {"control", 0x3B},
        {"option", 0x3A},
        {"command", 0x37},
        {"delete", 0x33},
        {"escape", 0x35},
        {"esc", 0x35},
        {"f1", 0x7A},
        {"f2", 0x78},
        {"f3", 0x63},
        {"f4", 0x76},
        {"f5", 0x60},
        {"f6", 0x61},
        {"f7", 0x62},
        {"f8", 0x64},
        {"f9", 0x65},
        {"f10", 0x6D},
        {"f11", 0x67},
        {"f12", 0x6F},
        {"spacebar", 0x31},
        {"space", 0x31},
        {"enter", 0x24},
        {"return", 0x24}
    };

    CGDirectDisplayID displayID;
    CGFloat displayHeight;
    CGFloat displayWidth;
    
public:
    MacController ();

    bool capturePortionOfScreen(std::string identifier) override;
    void click(unsigned xpos, unsigned ypos, int clickCount) override;
    void moveCursor(unsigned xpos, unsigned ypos) override;
    void say(std::string text) override;
    void pressButton(std::string text) override;
    void sendKeys(std::string text) override;
    int systemCommand(std::string text) override;

    unsigned char* getScreenCapture();
    int getDisplayHeight();
    int getDisplayWidth();
};


