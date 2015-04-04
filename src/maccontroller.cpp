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

#include "maccontroller.h"

MacController::MacController()
{
    // this sets up the screen variables needed for screencaptures
    displayID = CGMainDisplayID();
    CGRect mainMonitor = CGDisplayBounds(displayID);
    displayHeight = CGRectGetHeight(mainMonitor);
    displayWidth = CGRectGetWidth(mainMonitor);
}

int MacController::getDisplayHeight()
{
   return displayHeight;
}

int MacController::getDisplayWidth()
{
   return displayWidth;
}

// Returns as a screen capture as an unsigned char array
unsigned char* MacController::getScreenCapture()
{
    // Capture the screen as a CGImageRef
    CGImageRef testImage = CGDisplayCreateImageForRect (displayID, CGRectMake (0, 0, displayWidth, displayHeight) );

    // Convert the CGImageRef into a CFDataRef
    CGDataProviderRef provider = CGImageGetDataProvider(testImage);
    CFDataRef imageData = CGDataProviderCopyData(provider);

    long dataLength = CFDataGetLength(imageData);

    // Copy the data into an unsigned char array (range 0-255)
    // CFRangeMake tells it we want all data from the first byte to the last
    unsigned char* data = new  unsigned char[dataLength];
    CFDataGetBytes (imageData, CFRangeMake(0,dataLength), data);

    return data;
}

// This utilises the in-built mac screen capture to capture a portion of the screen
bool MacController::capturePortionOfScreen(std::string identifier)
{
    std::string cmd = "screencapture -s -x \"../Resources/img/" + identifier + ".jpg\"";
    const char * ccmd = cmd.c_str();
    if (system(ccmd) == 0)
        return true;
    else
        return false;
}

void MacController::click(unsigned xpos, unsigned ypos, int clickCount)
{
    CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStateHIDSystemState);
    // Left button down
    CGEventRef click1_down = CGEventCreateMouseEvent(
        source, kCGEventLeftMouseDown,
        CGPointMake(xpos, ypos),
        kCGMouseButtonLeft
    );
    ++eventNumber;
    CGEventSetIntegerValueField(click1_down, kCGMouseEventNumber, eventNumber);
    CGEventSetIntegerValueField(click1_down, kCGMouseEventClickState, clickCount);
    CGEventPost(kCGHIDEventTap, click1_down);

    // Left button up
    CGEventRef click1_up = CGEventCreateMouseEvent(
        source, kCGEventLeftMouseUp,
        CGPointMake(xpos, ypos),
        kCGMouseButtonLeft
    );
    CGEventSetIntegerValueField(click1_up, kCGMouseEventNumber, eventNumber);
    CGEventSetIntegerValueField(click1_up, kCGMouseEventClickState, clickCount);
    
    // sleep for 100 milliseconds
    // this is necessary to ensure all quick clicking works correctly
    struct timespec time;
    time.tv_sec = 0;
    time.tv_nsec = 1000000;
    nanosleep(&time,NULL);

    CGEventPost(kCGHIDEventTap, click1_up);
    CFRelease(click1_up);
    CFRelease(click1_down);
}

void MacController::moveCursor(unsigned xpos, unsigned ypos)
{
    CGEventRef move2 = CGEventCreateMouseEvent(
        NULL, kCGEventMouseMoved,
        CGPointMake(xpos, ypos),
        kCGMouseButtonLeft // ignored
    );
    CGEventPost(kCGHIDEventTap, move2);
    CFRelease(move2);
}

// this function should use the default voice, unless otherwise specified.
void MacController::say(std::string text)
{
    std::string cmd = "say -v \"Kate\" \"" + text + "\"";
    const char * ccmd = cmd.c_str();
    system(ccmd);
}

void MacController::pressButton(std::string text)
{
    if (buttonmap.find(text) != buttonmap.end())
    {
        int button = buttonmap[text];
        CGEventRef keyd = CGEventCreateKeyboardEvent(
                NULL, (CGKeyCode)button, true);
        CGEventRef keyu = CGEventCreateKeyboardEvent(
                NULL, (CGKeyCode)button, false);


        CGEventPost(kCGHIDEventTap, keyd);
        CFRelease(keyd);
        CGEventPost(kCGHIDEventTap, keyu);
        CFRelease(keyu);
    }
}

void MacController::sendKeys(std::string text)
{
    // iterate through each character and send keys
    for (size_t i = 0; i < text.length(); i++)
    {
        std::pair<int, bool> button = charmap[text[i]];
        CGEventRef keyd = CGEventCreateKeyboardEvent(
                NULL, (CGKeyCode)button.first, true);
        CGEventRef keyu = CGEventCreateKeyboardEvent(
                NULL, (CGKeyCode)button.first, false);

        // flag the shift key if needed
        if (button.second)
            CGEventSetFlags(keyd, kCGEventFlagMaskShift);

        CGEventPost(kCGHIDEventTap, keyd);
        CFRelease(keyd);
        CGEventPost(kCGHIDEventTap, keyu);
        CFRelease(keyu);
        if (button.second)
        {
            CGEventRef shiftu = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)56, false);
            CGEventPost(kCGSessionEventTap, shiftu);
            CFRelease(shiftu);
        }
        usleep(300); // for performance, check how slow this needs to be
    }
}

int MacController::systemCommand(std::string cmd)
{
    const char * ccmd = cmd.c_str();
    return system(ccmd);
}
