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

#include "agent.h"

using namespace std;
using namespace std::placeholders;

Agent::Agent(Controller *controller) 
{
    c = controller;

    auto fp = bind(&Controller::clickOnImage, ref(*c), _1);
    PrimitiveAction *actClick = new PrimitiveAction(0, fp, "click ?", c);
    fp = bind(&Controller::sendKeys, ref(*c), _1);
    PrimitiveAction *actType = new PrimitiveAction(1, fp, "type ?", c);
    fp = bind(&Controller::systemCommand, ref(*c), _1);
    PrimitiveAction *actSystem = new PrimitiveAction(2, fp, "system ?", c);
    fp = bind(&Controller::wait, ref(*c), _1);
    PrimitiveAction *actWait = new PrimitiveAction(3, fp, "wait ? seconds", c);
    fp = bind(&Controller::waitUntilVisible, ref(*c), _1);
    PrimitiveAction *actWaitUntil = new PrimitiveAction(4, fp, "wait until ? is visible", c);
    fp = bind(&Controller::hover, ref(*c), _1);
    PrimitiveAction *actHover = new PrimitiveAction(5, fp, "find ?", c);
    fp = bind(&Controller::followWhileVisible, ref(*c), _1);
    PrimitiveAction *actFollow = new PrimitiveAction(6, fp, "follow ?", c);
    fp = bind(&Controller::say, ref(*c), _1);
    PrimitiveAction *actSay = new PrimitiveAction(7, fp, "say ?", c);
    fp = bind(&Controller::pressButton, ref(*c), _1);
    PrimitiveAction *actPress = new PrimitiveAction(8, fp, "press ?", c);

    fp = bind(&Controller::showGUI, ref(*c), _1);
    PrimitiveAction *actShowGUI = new PrimitiveAction(9, fp, "showGUI", c);
    fp = bind(&Controller::hideGUI, ref(*c), _1);
    PrimitiveAction *actHideGUI = new PrimitiveAction(10, fp, "hideGUI", c);

    addAction(actClick);
    addAction(actType);
    addAction(actSystem);
    addAction(actWait);
    addAction(actWaitUntil);
    addAction(actHover);
    addAction(actFollow);
    addAction(actSay);
    addAction(actPress);
    addAction(actShowGUI);
    addAction(actHideGUI);
    
    loadPlansFromFile();
}

void Agent::addAction(Action* action)
{
    vector<string> formatSplit = Util::split(action->format,' ');

    actions.insert(pair<string,Action*>(formatSplit[0],action));

    actionIDMap[action->actionID] = action;
}

void Agent::addFlowControl(FlowControl* flowControl)
{
    vector<string> formatSplit = Util::split(flowControl->format,' ');

    flowControls.insert(pair<string,FlowControl*>(formatSplit[0],flowControl));
}

bool Agent::loadPlansFromFile()
{
    string line;
    vector<string> lineSplit;
    vector<string> argsSplit;
    long int currID;
    vector<Plan*> newActions; // actions currently being made
    Plan *currPlan;
    Action *currAction;
    flow_control_t validFlowControl;

    int lineNumber = 0;

    ifstream planFile ("../Resources/plans.txt");
    if (planFile.is_open())
    {
        while (getline (planFile,line) )
        {
            lineNumber++; // used for error reporting
            if (line != "")
            {
                lineSplit = Util::split(line,',');

                if (!Util::stringToLongInt(lineSplit[0], currID) || currID < 100)
                    goto planNotValid;

                currPlan = new Plan(currID, lineSplit[1], c);
                newActions.push_back(currPlan);
                lastActionID = currID;

                for (size_t i = 2; i < lineSplit.size(); i++)
                {
                    if (Util::stringToLongInt(lineSplit[i], currID))
                    {
                        if (lineSplit.size() < (i+1))
                            goto planNotValid;
                        currAction = actionIDMap[currID];
                        argsSplit = Util::split(lineSplit[++i],'+');
                        currPlan->addToPlan(*currAction, argsSplit);
                    } else {
                        vector<string> args;
                        if (findValidFlowControl(lineSplit[i], validFlowControl, args))
                        {
                            FlowControl *fc = new FlowControl(validFlowControl, lineSplit[i], c);
                            args.push_back("?0");
                            currPlan->addToPlan(*fc, args);
                            newActions.push_back(fc);
                            currPlan = fc;
                        } else if (lineSplit[i] == "done" && newActions.size() > 1) {
                            newActions.pop_back();
                            currPlan = newActions.back();
                        } else {
                            goto planNotValid;
                        }
                    }
                }

                addAction(currPlan);

            }
            continue;
        planNotValid:
            cerr << "Unable to add the plan on line " << to_string(lineNumber) << ".\n";

        }
        planFile.close();
    } else {
        cerr << "Unable to load saved plans from file.\n";
        return false;
    }
    return true;
}

std::vector<string> Agent::handleInput(string input)
{
    vector<pair<Action*,vector<string>>> validActions;
    flow_control_t validFlowControl;
    Plan *currPlan;

    if (input == "onLoad")
        return {"What can I help you with today?"};
    else if (input == "cancel")
    {
        state = IDLE;
        newActions.erase(newActions.begin(), newActions.end());
        return {"What can I help you with?"};
    }
    else if (input == "")
        return {""};

    switch (state)
    {
    case IDLE:
        if (input == "new command")
        {
            state = NP_FORMAT;
            return {"What will the format of this new command be?"};
        }
        // create a vector to store all actions that match input format
        validActions = findValidActions(input);
        switch (validActions.size()) 
        {
        case 0:
            // no valid actions, ask to create new command
            state = NP_CREATE_YN;
            return {"I do not know how to do this. Would you like to teach me?"};
        case 1:
            if(c->usingGUI)
                c->hideGUI({});
            //sleep(1);
            validActions[0].first->run(validActions[0].second);
            return {""};
        default:
            // TODO complete this 
            return {"Multiple valid options.. please select one:"};
        }
        break;
    case FC_CREATE_YN:
    case NP_CREATE_YN:
        if (input == "yes" || input == "y")
        {
            state = NP_FORMAT;
            return {"What will the format of this new command be?"};
        } else if (input == "no" || input == "n")
        {
            if (newActions.size() == 0)
            {
                state = IDLE;
                return {"What else can I help you with?"};
            } else {
                if (state == NP_CREATE_YN)
                    state = NP_ADD_ACTION;
                else
                    state = FC_ADD_ACTION;
                return {"What action should I perform next then?"};
            }
        } else {
            // state remains the same, until yes or no is selected
            return {"Please answer either yes or no!"};
        }
        break;
    case NP_FORMAT:
        currPlan = new Plan(++lastActionID, input, c);
        newActions.push_back(currPlan);
        state = NP_ADD_ACTION;
        return {"What action should I perform first?"};
        break;
    case FC_ADD_ACTION: // both cases jump here
    case NP_ADD_ACTION:
        currPlan = newActions.back();
        if (input == "done")
        {
            newActions.pop_back();
            if (newActions.size() == 0)
            {
                if (!currPlan->savePlanToFile())
                    return {"Unexpected error: Unable to save new plan to file."};
                addAction(currPlan);
                state = IDLE;
                return {"New command learnt. What else can I help you with?"};
            } else {
                if (state == FC_ADD_ACTION)
                {
                    state = NP_ADD_ACTION;
                    return {"Added condition statement to command. What next?"};
                } else {
                    if (!currPlan->savePlanToFile())
                        return {"Unexpected error: Unable to save new plan to file."};
                    addAction(currPlan);
                    currPlan = newActions.back();
                    input = inputForNewPlan;
                    validActions = findValidActions(input);
                    switch (validActions.size()) 
                    {
                    case 0:
                        return {"The format of this new command does not match "
                                "your last input. To add this command, use the "
                                "format ''"+currPlan->format+"'."};
                    case 1:
                        currPlan->addToPlan(*validActions[0].first, validActions[0].second);
                        return {"New command learnt. What action should I perform next?"};
                    default:
                        // TODO complete this 
                        return {"Multiple valid options for new command.. should autoselect:"};
                    }
                 }
            }
        } else {
            vector<string> args;
            if (findValidFlowControl(input, validFlowControl, args))
            {
                FlowControl *fc = new FlowControl(validFlowControl, input, c);
                args.push_back("?0");
                currPlan->addToPlan(*fc, args);
                newActions.push_back(fc);
                state = FC_ADD_ACTION;
                return {"What actions should I perform under this condition?"};
            }
            validActions = findValidActions(input);
            switch (validActions.size()) 
            {
            case 0:
                state = NP_CREATE_YN;
                inputForNewPlan = input;
                return {"I do not know how to do this. Would you like to teach me this at the same time?"};
            case 1:
                currPlan->addToPlan(*validActions[0].first, validActions[0].second);
                return {"What action should I perform next?"};
            default:
                // TODO complete this 
                return {"Multiple valid options.. please select one:"};
            }
        }
        break;
    }
}

vector<pair<Action*,vector<string>>> Agent::findValidActions(string input)
{
    vector<pair<Action*,vector<string>>> validActions;

    // put each word from the input into a vector
    vector<string> inputSplit = Util::split(input,' ');

    // all actions are stored in a multimap, with the first word of its format
    // as it's key. Use equal_range to retrieve all formats with the same key
    pair <multimap<string,Action*>::iterator,
                                    multimap<string,Action*>::iterator> ret;
    ret = actions.equal_range(inputSplit[0]);

    // loop through each of these actions and compare each format against the
    // input to find valid actions.
    for (multimap<string,Action*>::iterator it=ret.first; it!=ret.second; ++it)
    {
        vector<string> formatSplit = Util::split(it->second->format,' ');
        vector<string> args;
        if (compareInputFormat(inputSplit, formatSplit, args))
        {
            validActions.push_back({it->second,args});
        }
    }

    return validActions;
}

bool Agent::findValidFlowControl(string input, flow_control_t &fc, vector<string> &args)
{
    // put each word from the input into a vector
    vector<string> inputSplit = Util::split(input,' ');

    for (pair<flow_control_t,string> currFC : FlowControl::fcMap)
    {
        vector<string> formatSplit = Util::split(currFC.second,' ');
        if (compareInputFormat(inputSplit, formatSplit, args))
        {
            fc = currFC.first;
            return true;
        }
    }

    return false;
}


// checks if the input matches an actions format, and puts input arguments into args
bool Agent::compareInputFormat(vector<string> &inputSplit,
                               vector<string> &formatSplit, vector<string> &args)
{
    bool activeWildCard = false; // if the algorithm has just seen a wildcard
    string currArg = "";

    for(size_t formatIndex = 0, inputIndex = 0; formatIndex < formatSplit.size(); formatIndex++)
    {
        // if both input is past its last word, return false
        if (inputIndex == (inputSplit.size()))
        {
            return false;
        }
        if (formatSplit[formatIndex].compare("?") == 0)
        {
            activeWildCard = true;
            currArg = inputSplit[inputIndex];
            inputIndex++;
            // if format is on its last word, return true
            if (formatIndex == (formatSplit.size()-1))
            {
                while (inputIndex < inputSplit.size())
                {
                    currArg += " " + inputSplit[inputIndex];
                    inputIndex++;
                }
                args.push_back(currArg); 
                return true;
            }
            continue;
        }
        if (formatSplit[formatIndex].compare(inputSplit[inputIndex]) == 0)
        {
            if (activeWildCard)
            {
                args.push_back(currArg); 
                currArg = "";
                activeWildCard = false;
            }
            // if both format and input are on matching last words, return true
            if ((inputIndex == (inputSplit.size()-1))
                    && (formatIndex == (formatSplit.size()-1)))
            {
                return true;
            }
            inputIndex++;
            continue;
        }
        else if (activeWildCard)
        {
            currArg += " " + inputSplit[inputIndex];
            formatIndex--;
            inputIndex++;
            if (inputIndex == inputSplit.size())
                continue;
        }
        else 
        {
            return false;
        }
    }

    return false;
}
