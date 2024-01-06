/*
 *  Alien-Macro
 *  Receives Macro keypresses and translates to a keycode that AutoHotKey can understand.
 *  Copyright (C) 2023 mscreations
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *  CURRENT STATUS: Proof of Concept. Still needs a lot of refinement and should not be used
 *      in normal day to day use.
 *
 */

#include <regex>
#include "Alien-Macros.h"
#include "argparse.h"

int main(int argc, char* argv[])
{
    argparse::Parser parser;

    auto& vid = parser.AddArg<std::string>("vid", 'v', "Target VID").Default(AW_KB_VID);
    auto& pid = parser.AddArg<std::string>("pid", 'p', "Target PID").Default(AW_KB_PID);
    parser.ParseArgs(argc, argv);

    std::regex re("(?:0x)[0-9a-fA-F]{4}");
    if (!std::regex_match(*pid, re) || !std::regex_match(*vid, re))
    {
        std::cerr << "VID/PID is invalid. Please make sure it is in the format 0xXXXX where XXXX is the hexadecimal VID/PID." << std::endl;
        return -1;
    }

    WORD targetVID = (WORD)std::stoi(*vid, nullptr, 16);
    WORD targetPID = (WORD)std::stoi(*pid, nullptr, 16);

    std::cout << "Alien Macros - Version " << GetAppVersion() << std::endl;

    return StartMonitor(targetVID, targetPID);
}