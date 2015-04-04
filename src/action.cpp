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

#include "action.h"

using namespace std;

PrimitiveAction::PrimitiveAction(int id, function<bool(vector<string>)> ptr,
                 string f, Controller *controller) : Action(id, f, controller)
{
    funcPtr = ptr;
}

bool PrimitiveAction::run(vector<string> args)
{
    return funcPtr(args); 
}

void Plan::addToPlan(Action &newAction, vector<string> args)
{
    actionList.push_back({&newAction, args});
}

bool Plan::run(vector<string> args)
{
    vector<vector<string>> currPlanArgs;

    if (!getCurrPlanArgs(currPlanArgs, args))
        return false;


    // run each action in the plan, if one fails, stop and return false
    for (size_t i = 0; i < actionList.size(); i++)
    {
        if (!actionList[i].first->run(currPlanArgs[i]))
            return false;
    }
    return true;
}

bool Plan::savePlanToFile()
{
    ofstream planFile ("../Resources/plans.txt", ios::app);
    if (planFile.is_open())
    {
        planFile << "\n" << actionID << "," << format;

        for (size_t i = 0; i < actionList.size(); i++)
        {
            planFile <<","<<actionList[i].first->getSaveFormat(actionList[i].second);
        }

        planFile.close();
    } else {
        return false;
    }
    return true;
}

string Action::getSaveFormat(vector<string> args)
{
    string saveFormat = to_string(actionID);
    string argsFormat = "";
    for (size_t j = 0; j < args.size(); j++)
    {
        if (j == 0) 
            argsFormat = args[j];
        else
            argsFormat += "+" + args[j];
    }
    saveFormat += "," + argsFormat;
    return saveFormat;
}

bool Plan::getCurrPlanArgs(vector<vector<string>> &currPlanArgs,
                                             vector<string> args)
{
    vector<vector<string>> newPlanArgs;

    // for each action in the plan
    for (std::pair <Action*,std::vector<std::string>> currAction: actionList)
    {
        // grab it's original arguments
        vector<string> currArgs = currAction.second;
        for (size_t i = 0; i < currArgs.size(); i++)
        {
            if (currArgs[i].at(0) == '?')
            {
                long int argId;
                if (Util::stringToLongInt(currArgs[i].substr(1,
                                                 currArgs[i].length() - 1), argId))
                {
                    // if argsId = 0, this must be a flow control, add a copy of each
                    if (argId > 0)
                    {
                        currArgs[i] = args[argId-1];
                    } else {
                        currArgs.pop_back();
                        for (string a: args)
                        {
                            currArgs.push_back(a);
                        }
                    }
                } else {
                    return false;
                }
            }
        }
        newPlanArgs.push_back(currArgs);
    }
    currPlanArgs = newPlanArgs;
    return true;
}

map<flow_control_t,string> FlowControl::fcMap = {
        {IF_VISIBLE, "if ? is visible"},
        {IF_NOT_VISIBLE, "if ? is not visible"},
        {REPEAT_FOR_SECONDS, "repeat for ? seconds"},
        {REPEAT_TIMES, "repeat ? times"},
        {WHILE_VISIBLE, "while ? is visible"}
    };

bool FlowControl::run(vector<string> args)
{
    vector<vector<string>> currPlanArgs;

    bool condition;
    double confidence;
    pair <int,int> location;
    long int parsedInt;
    clock_t begin;
    double elapsed_time;

    vector<string> parentPlanArgs(args.begin()+1, args.end());

    if (!getCurrPlanArgs(currPlanArgs, parentPlanArgs))
        return false;

    switch (flow_control)
    {
    case IF_VISIBLE:
    case IF_NOT_VISIBLE:
        // return false if the image does not exist, and the user does
        // not specify what the image looks like
        if(!c->findImageOnScreen(args[0], location, confidence))
            return false;

        // confidence must be above 0.6 to be considered visible
        condition = (confidence > 0.6);
        if (flow_control == IF_NOT_VISIBLE)
            condition = !condition;

        if (condition)
        {
            for (size_t i = 0; i < actionList.size(); i++)
            {
                if (!actionList[i].first->run(currPlanArgs[i]))
                    return false;
            }
        }
        return true;
        break;
    case REPEAT_FOR_SECONDS:
        if (!Util::stringToLongInt(args[0], parsedInt))
            return false;
        begin = clock();
        elapsed_time = double(clock() - begin)/CLOCKS_PER_SEC; 
        while (elapsed_time < parsedInt)
        {
            for (size_t j = 0; j < actionList.size(); j++)
            {
                if (!actionList[j].first->run(currPlanArgs[j]))
                    return false;
            }
            elapsed_time = double(clock() - begin)/CLOCKS_PER_SEC; 
            //cout << elapsed_time << endl;
        }
        return true;
        break;
    case REPEAT_TIMES:
        if (!Util::stringToLongInt(args[0], parsedInt))
            return false;
        for (int i = 0; i < parsedInt; i++)
        {
            for (size_t j = 0; j < actionList.size(); j++)
            {
                if (!actionList[j].first->run(currPlanArgs[j]))
                    return false;
            }
        }
        return true;
        break;
    case WHILE_VISIBLE:
    case WHILE_NOT_VISIBLE:
        if(!c->findImageOnScreen(args[0], location, confidence))
            return false;
        condition = (confidence > 0.6);
        if (flow_control == WHILE_NOT_VISIBLE)
            condition = !condition;
        while (condition)
        {
            for (size_t i = 0; i < actionList.size(); i++)
            {
                if (!actionList[i].first->run(currPlanArgs[i]))
                    return false;
            }
            c->findImageOnScreen(args[0], location, confidence);
        }
        return true;
        break;
    case UNDEFINED:
        return false;
        break;
    }
    return false;
}

string FlowControl::getSaveFormat(vector<string> args)
{
    (void)args;
    string saveFormat = origInput;
    for (size_t i = 0; i < actionList.size(); i++)
    {
        saveFormat +=","+actionList[i].first->getSaveFormat(actionList[i].second);
    }
    saveFormat += ",done";
    return saveFormat;
}

