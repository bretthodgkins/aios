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
#include <string>
#include <map>
#include <vector>
#include <new>
#include <iterator> // std::distance
#include "action.h"
#include "controller.h"
#include "util.h"

enum agent_states {IDLE, NP_CREATE_YN, NP_FORMAT, NP_ADD_ACTION, FC_ADD_ACTION, FC_CREATE_YN};

class Agent
{
    public:
        Agent(Controller *c);
        std::vector<string> handleInput(std::string input);

    private:
        Controller *c;
        agent_states state = IDLE;
        std::multimap<std::string,Action*> actions; 
        std::map<int,Action*> actionIDMap; 
        std::multimap<std::string,FlowControl*> flowControls; 
        vector<Plan*> newActions; // actions currently being made

        // keep track of the highest action id, so that new actions can be added sequentially
        // default = 99, so that we can have up to 99 non-user defined actions
        int lastActionID = 99;

        // when creating multiple plans at once, this stores the original input
        // for the newest plan, to be added to the first plan once created
        std::string inputForNewPlan;

        void addAction(Action*);
        void addFlowControl(FlowControl*);

        bool loadPlansFromFile();

        vector<pair<Action*,vector<std::string>>> findValidActions(std::string input);
        bool findValidFlowControl(std::string input, flow_control_t &fc,
                                  vector<std::string> &args);
        void findValidActions(std::string input, vector<pair<Action*,
                              vector<std::string>>> &validActions);
        bool compareInputFormat(std::vector<std::string> &inputSplit,
            std::vector<std::string> &formatSplit, std::vector<std::string> &args);
};
