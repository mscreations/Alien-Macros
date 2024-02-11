/*
 *  Alien-Macros
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
 */

#pragma once

#include <string>
#include <unordered_map>

#ifdef VK_CANCEL
#error "NAME CLASH"
#endif

class Utils
{
    static void ResetCin();
    static bool ConvertToNumeric(const std::string& userInput, unsigned int& response);

public:
    inline static std::unordered_map<int, std::string> virtualKeyStrings = { {0x03, "VK_CANCEL"}, {0x08, "VK_BACK"},
        {0x09, "VK_TAB"}, {0x0C, "VK_CLEAR"}, {0x0D, "VK_RETURN"}, {0x13, "VK_PAUSE"}, {0x14, "VK_CAPITAL"},
        {0x1B, "VK_ESCAPE"}, {0x20, "VK_SPACE"}, {0x21, "VK_PRIOR"}, {0x22, "VK_NEXT"}, {0x23, "VK_END"},
        {0x24, "VK_HOME"}, {0x25, "VK_LEFT"}, {0x26, "VK_UP"}, {0x27, "VK_RIGHT"}, {0x28, "VK_DOWN"},
        {0x2C, "VK_SNAPSHOT"}, {0x2D, "VK_INSERT"}, {0x2E, "VK_DELETE"}, {0x2F, "VK_HELP"}, {0x30, "VK_0"},
        {0x31, "VK_1"}, {0x32, "VK_2"}, {0x33, "VK_3"}, {0x34, "VK_4"}, {0x35, "VK_5"}, {0x36, "VK_6"}, {0x37, "VK_7"},
        {0x38, "VK_8"}, {0x39, "VK_9"}, {0x41, "VK_A"}, {0x42, "VK_B"}, {0x43, "VK_C"}, {0x44, "VK_D"}, {0x45, "VK_E"},
        {0x46, "VK_F"}, {0x47, "VK_G"}, {0x48, "VK_H"}, {0x49, "VK_I"}, {0x4A, "VK_J"}, {0x4B, "VK_K"}, {0x4C, "VK_L"},
        {0x4D, "VK_M"}, {0x4E, "VK_N"}, {0x4F, "VK_O"}, {0x50, "VK_P"}, {0x51, "VK_Q"}, {0x52, "VK_R"}, {0x53, "VK_S"},
        {0x54, "VK_T"}, {0x55, "VK_U"}, {0x56, "VK_V"}, {0x57, "VK_W"}, {0x58, "VK_X"}, {0x59, "VK_Y"}, {0x5A, "VK_Z"},
        {0x5B, "VK_LWIN"}, {0x5C, "VK_RWIN"}, {0x5D, "VK_APPS"}, {0x5F, "VK_SLEEP"}, {0x6A, "VK_MULTIPLY"},
        {0x6B, "VK_ADD"}, {0x6D, "VK_SUBTRACT"}, {0x6F, "VK_DIVIDE"}, {0x70, "VK_F1"}, {0x71, "VK_F2"}, {0x72, "VK_F3"},
        {0x73, "VK_F4"}, {0x74, "VK_F5"}, {0x75, "VK_F6"}, {0x76, "VK_F7"}, {0x77, "VK_F8"}, {0x78, "VK_F9"},
        {0x79, "VK_F10"}, {0x7A, "VK_F11"}, {0x7B, "VK_F12"}, {0x7C, "VK_F13"}, {0x7D, "VK_F14"}, {0x7E, "VK_F15"},
        {0x7F, "VK_F16"}, {0x80, "VK_F17"}, {0x81, "VK_F18"}, {0x82, "VK_F19"}, {0x83, "VK_F20"}, {0x84, "VK_F21"},
        {0x85, "VK_F22"}, {0x86, "VK_F23"}, {0x87, "VK_F24"}, {0x90, "VK_NUMLOCK"}, {0x91, "VK_SCROLL"},
        {0xA0, "VK_LSHIFT"}, {0xA1, "VK_RSHIFT"}, {0xA2, "VK_LCONTROL"}, {0xA3, "VK_RCONTROL"}, {0xA4, "VK_LMENU"},
        {0xA5, "VK_RMENU"}, {0xA6, "VK_BROWSER_BACK"}, {0xA7, "VK_BROWSER_FORWARD"}, {0xA8, "VK_BROWSER_REFRESH"},
        {0xA9, "VK_BROWSER_STOP"}, {0xAA, "VK_BROWSER_SEARCH"}, {0xAB, "VK_BROWSER_FAVORITES"}, {0xAC, "VK_BROWSER_HOME"},
        {0xAD, "VK_VOLUME_MUTE"}, {0xAE, "VK_VOLUME_DOWN"}, {0xAF, "VK_VOLUME_UP"}, {0xB0, "VK_MEDIA_NEXT_TRACK"},
        {0xB1, "VK_MEDIA_PREV_TRACK"}, {0xB2, "VK_MEDIA_STOP"}, {0xB3, "VK_MEDIA_PLAY_PAUSE"}, {0xB4, "VK_LAUNCH_MAIL"},
        {0xB5, "VK_LAUNCH_MEDIA_SELECT"}, {0xB6, "VK_LAUNCH_APP1"}, {0xB7, "VK_LAUNCH_APP2"}, {0xBA, "VK_OEM_1"},
        {0xBB, "VK_OEM_PLUS"}, {0xBC, "VK_OEM_COMMA"}, {0xBD, "VK_OEM_MINUS"}, {0xBE, "VK_OEM_PERIOD"},
        {0xBF, "VK_OEM_2"}, {0xC0, "VK_OEM_3"}, {0xC1, "VK_ABNT_C1"}, {0xC2, "VK_ABNT_C2"}, {0xDB, "VK_OEM_4"},
        {0xDC, "VK_OEM_5"}, {0xDD, "VK_OEM_6"}, {0xDE, "VK_OEM_7"} };

    enum class VirtualKeys : short
    {
        VK_CANCEL = 0x03, VK_BACK = 0x08, VK_TAB = 0x09, VK_CLEAR = 0x0C, VK_RETURN = 0x0D, VK_PAUSE = 0x13, VK_CAPITAL = 0x14,
        VK_ESCAPE = 0x1B, VK_SPACE = 0x20, VK_PRIOR, VK_NEXT, VK_END, VK_HOME, VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN,
        VK_SNAPSHOT = 0x2C, VK_INSERT, VK_DELETE, VK_HELP,
        VK_0 = 0x30, VK_1, VK_2, VK_3, VK_4, VK_5, VK_6, VK_7, VK_8, VK_9,
        VK_A = 0x41, VK_B, VK_C, VK_D, VK_E, VK_F, VK_G, VK_H, VK_I, VK_J, VK_K, VK_L, VK_M, VK_N, VK_O,
        VK_P, VK_Q, VK_R, VK_S, VK_T, VK_U, VK_V, VK_W, VK_X, VK_Y, VK_Z,
        VK_LWIN = 0x5B, VK_RWIN, VK_APPS, VK_SLEEP = 0x5F, VK_MULTIPLY = 0x6A, VK_ADD, VK_SUBTRACT = 0x6D, VK_DIVIDE = 0x6F,
        VK_F1 = 0x70, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12, VK_F13, VK_F14, VK_F15,
        VK_F16, VK_F17, VK_F18, VK_F19, VK_F20, VK_F21, VK_F22, VK_F23, VK_F24, VK_NUMLOCK = 0x90, VK_SCROLL, VK_LSHIFT = 0xA0,
        VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, VK_LMENU, VK_RMENU, VK_BROWSER_BACK, VK_BROWSER_FORWARD, VK_BROWSER_REFRESH,
        VK_BROWSER_STOP, VK_BROWSER_SEARCH, VK_BROWSER_FAVORITES, VK_BROWSER_HOME, VK_VOLUME_MUTE, VK_VOLUME_DOWN, VK_VOLUME_UP,
        VK_MEDIA_NEXT_TRACK, VK_MEDIA_PREV_TRACK, VK_MEDIA_STOP, VK_MEDIA_PLAY_PAUSE, VK_LAUNCH_MAIL, VK_LAUNCH_MEDIA_SELECT,
        VK_LAUNCH_APP1, VK_LAUNCH_APP2, VK_OEM_1 = 0xBA, VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PERIOD, VK_OEM_2, VK_OEM_3,
        VK_ABNT_C1, VK_ABNT_C2, VK_OEM_4 = 0xDB, VK_OEM_5, VK_OEM_6, VK_OEM_7,
    };

    static bool AskResponse(const std::string& message, unsigned int& response, const unsigned int maxValue = UINT_MAX);
    static bool AskResponse(const std::string& message, const unsigned int firstOption, const unsigned int lastOption, unsigned int& response);
    static bool AskResponse(const std::string& message, bool yesDefault);
    static bool AskResponse(const std::string& message, std::string& response);
    static bool AskResponse(const std::string& message, char& response);
};


