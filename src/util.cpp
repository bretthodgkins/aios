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

#include "util.h"

using namespace std;

namespace Util
{
   // This will either -1 if not a real number
   bool stringToLongInt(const string &s, long int &num)
   {
       if (s.empty()) return false;

       char *p;
       long int rv = strtol(s.c_str(), &p, 10);
       if (*p != 0)
           return false;

       num = rv;
       return true;
   }

   void fixEscapeCharacters(string &s)
   {
       findAndReplaceString(s, "\\t", "\t");
       findAndReplaceString(s, "\\n", "\n");
   }

   void findAndReplaceString(string &s, const string strReplace,
                                         const string strReplaceWith)
   {
       while (s.find(strReplace) != string::npos)
       {
           s.replace(s.find(strReplace), strReplace.length(), strReplaceWith);
       }
   }

   vector<string> &split(const string &s, char delim, vector<string> &elems) {
       stringstream ss(s);
       string item;
       while (getline(ss, item, delim)) {
           elems.push_back(item);
       }
       return elems;
   }


   vector<string> split(const string &s, char delim) {
       vector<string> elems;
       split(s, delim, elems);
       return elems;
   }

   bool doesFileExist(string filename)
   {
       if (FILE *file = fopen(filename.c_str(), "r"))
       {
           fclose(file);
           return true;
       } else {
           return false;
       }
   }
}
