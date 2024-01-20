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

enum class VirtualKeys : short
{
    VKC_CANCEL = 0x03, VKC_BACK = 0x08, VKC_TAB = 0x09, VKC_CLEAR = 0x0C, VKC_RETURN = 0x0D, VKC_PAUSE = 0x13, VKC_CAPITAL = 0x14,
    VKC_ESCAPE = 0x1B, VKC_SPACE = 0x20, VKC_PRIOR, VKC_NEXT, VKC_END, VKC_HOME, VKC_LEFT, VKC_UP, VKC_RIGHT, VKC_DOWN,
    VKC_SNAPSHOT = 0x2C, VKC_INSERT, VKC_DELETE, VKC_HELP,
    VKC_0 = 0x30, VKC_1, VKC_2, VKC_3, VKC_4, VKC_5, VKC_6, VKC_7, VKC_8, VKC_9,
    VKC_A = 0x41, VKC_B, VKC_C, VKC_D, VKC_E, VKC_F, VKC_G, VKC_H, VKC_I, VKC_J, VKC_K, VKC_L, VKC_M, VKC_N, VKC_O,
    VKC_P, VKC_Q, VKC_R, VKC_S, VKC_T, VKC_U, VKC_V, VKC_W, VKC_X, VKC_Y, VKC_Z,
    VKC_LWIN = 0x5B, VKC_RWIN, VKC_APPS, VKC_SLEEP = 0x5F, VKC_MULTIPLY = 0x6A, VKC_ADD, VKC_SUBTRACT = 0x6D, VKC_DIVIDE = 0x6F,
    VKC_F1 = 0x70, VKC_F2, VKC_F3, VKC_F4, VKC_F5, VKC_F6, VKC_F7, VKC_F8, VKC_F9, VKC_F10, VKC_F11, VKC_F12, VKC_F13, VKC_F14, VKC_F15,
    VKC_F16, VKC_F17, VKC_F18, VKC_F19, VKC_F20, VKC_F21, VKC_F22, VKC_F23, VKC_F24, VKC_NUMLOCK = 0x90, VKC_SCROLL, VKC_LSHIFT = 0xA0,
    VKC_RSHIFT, VKC_LCONTROL, VKC_RCONTROL, VKC_LMENU, VKC_RMENU, VKC_BROWSER_BACK, VKC_BROWSER_FORWARD, VKC_BROWSER_REFRESH,
    VKC_BROWSER_STOP, VKC_BROWSER_SEARCH, VKC_BROWSER_FAVORITES, VKC_BROWSER_HOME, VKC_VOLUME_MUTE, VKC_VOLUME_DOWN, VKC_VOLUME_UP,
    VKC_MEDIA_NEXT_TRACK, VKC_MEDIA_PREV_TRACK, VKC_MEDIA_STOP, VKC_MEDIA_PLAY_PAUSE, VKC_LAUNCH_MAIL, VKC_LAUNCH_MEDIA_SELECT,
    VKC_LAUNCH_APP1, VKC_LAUNCH_APP2, VKC_OEM_1 = 0xBA, VKC_OEM_PLUS, VKC_OEM_COMMA, VKC_OEM_MINUS, VKC_OEM_PERIOD, VKC_OEM_2, VKC_OEM_3,
    VKC_ABNT_C1, VKC_ABNT_C2, VKC_OEM_4 = 0xDB, VKC_OEM_5, VKC_OEM_6, VKC_OEM_7,
};

std::string GetKeyName(short vk);