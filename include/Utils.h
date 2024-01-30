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

#ifdef VK_CANCEL
#error "NAME CLASH"
#endif

namespace Utils
{
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

    std::string GetKeyName(const short vk);

    bool AskResponse(const std::string& message, const unsigned int firstOption, const unsigned int lastOption, unsigned int& response);
    bool AskResponse(const std::string& message, bool yesDefault);
}