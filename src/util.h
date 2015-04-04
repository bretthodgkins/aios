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
#include <sstream>
#include <string>
#include <vector>

namespace Util
{
   bool doesFileExist(std::string filename);
   bool stringToLongInt(const std::string &s, long int &num);
   void fixEscapeCharacters(std::string &s);
   void findAndReplaceString(std::string &s, const std::string strReplace,
                                    const std::string strReplaceWith);
   std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
   std::vector<std::string> split(const std::string &s, char delim);
}
