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
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <time.h>
#include "controller.h"
#include "util.h"

enum flow_control_t {UNDEFINED, IF_VISIBLE, IF_NOT_VISIBLE, REPEAT_FOR_SECONDS, REPEAT_TIMES, WHILE_VISIBLE, WHILE_NOT_VISIBLE};

class Action
{
    public:
        int actionID;
        std::string format;
        virtual bool run(std::vector<std::string>) = 0;
        virtual string getSaveFormat(std::vector<std::string> args);

        Action(int id, std::string f, Controller *controller): actionID(id),
               format(f), c(controller) {}
        Action() {}
    protected:
        Controller *c;
};

class PrimitiveAction: public Action
{
    public:
        PrimitiveAction(int id, std::function<bool(std::vector<std::string>)>,
                        std::string, Controller *controller);
        bool run(std::vector<std::string>) override;
    private:
        std::function<bool(std::vector<std::string> args)> funcPtr;
};

class Plan: public Action
{
    public:
        void addToPlan(Action &newAction, std::vector<std::string> args);
        bool run(std::vector<std::string>) override;

        bool savePlanToFile();

        Plan(int id, std::string f, Controller *controller): Action(id, f,
             controller) {}
    protected:
        // Vector of actions, each with a matching vector of arguments
        std::vector<std::pair <Action*,std::vector<std::string>>> actionList;

        bool getCurrPlanArgs(vector<vector<string>> &currPlanArgs, vector<std::string> args);
};

class FlowControl: public Plan
{
    public:
        std::string origInput;
        static std::map<flow_control_t,std::string> fcMap;

        bool run(std::vector<std::string>) override;

        // the format is determined by flow_control_t
        FlowControl(flow_control_t fc, std::string input, Controller *controller): Plan(-1,fcMap[fc], controller), origInput(input), flow_control(fc) {}

    protected:
        string getSaveFormat(std::vector<std::string> args) override;

    private:
        flow_control_t flow_control;
};
