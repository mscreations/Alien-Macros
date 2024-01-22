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

#include "Utils.h"

std::string GetKeyName(short vk)
{
    std::string ret{};

    switch (static_cast<VirtualKeys>(vk))
    {
        case VirtualKeys::VK_CANCEL: ret = "VK_CANCEL"; break;
        case VirtualKeys::VK_BACK: ret = "VK_BACK"; break;
        case VirtualKeys::VK_TAB: ret = "VK_TAB"; break;
        case VirtualKeys::VK_CLEAR: ret = "VK_CLEAR"; break;
        case VirtualKeys::VK_RETURN: ret = "VK_RETURN"; break;
        case VirtualKeys::VK_PAUSE: ret = "VK_PAUSE"; break;
        case VirtualKeys::VK_CAPITAL: ret = "VK_CAPITAL"; break;
        case VirtualKeys::VK_ESCAPE: ret = "VK_ESCAPE"; break;
        case VirtualKeys::VK_SPACE: ret = "VK_SPACE"; break;
        case VirtualKeys::VK_PRIOR: ret = "VK_PRIOR"; break;
        case VirtualKeys::VK_NEXT: ret = "VK_NEXT"; break;
        case VirtualKeys::VK_END: ret = "VK_END"; break;
        case VirtualKeys::VK_HOME: ret = "VK_HOME"; break;
        case VirtualKeys::VK_LEFT: ret = "VK_LEFT"; break;
        case VirtualKeys::VK_UP: ret = "VK_UP"; break;
        case VirtualKeys::VK_RIGHT: ret = "VK_RIGHT"; break;
        case VirtualKeys::VK_DOWN: ret = "VK_DOWN"; break;
        case VirtualKeys::VK_SNAPSHOT: ret = "VK_SNAPSHOT"; break;
        case VirtualKeys::VK_INSERT: ret = "VK_INSERT"; break;
        case VirtualKeys::VK_DELETE: ret = "VK_DELETE"; break;
        case VirtualKeys::VK_HELP: ret = "VK_HELP"; break;
        case VirtualKeys::VK_0: ret = "VK_0"; break;
        case VirtualKeys::VK_1: ret = "VK_1"; break;
        case VirtualKeys::VK_2: ret = "VK_2"; break;
        case VirtualKeys::VK_3: ret = "VK_3"; break;
        case VirtualKeys::VK_4: ret = "VK_4"; break;
        case VirtualKeys::VK_5: ret = "VK_5"; break;
        case VirtualKeys::VK_6: ret = "VK_6"; break;
        case VirtualKeys::VK_7: ret = "VK_7"; break;
        case VirtualKeys::VK_8: ret = "VK_8"; break;
        case VirtualKeys::VK_9: ret = "VK_9"; break;
        case VirtualKeys::VK_A: ret = "VK_A"; break;
        case VirtualKeys::VK_B: ret = "VK_B"; break;
        case VirtualKeys::VK_C: ret = "VK_C"; break;
        case VirtualKeys::VK_D: ret = "VK_D"; break;
        case VirtualKeys::VK_E: ret = "VK_E"; break;
        case VirtualKeys::VK_F: ret = "VK_F"; break;
        case VirtualKeys::VK_G: ret = "VK_G"; break;
        case VirtualKeys::VK_H: ret = "VK_H"; break;
        case VirtualKeys::VK_I: ret = "VK_I"; break;
        case VirtualKeys::VK_J: ret = "VK_J"; break;
        case VirtualKeys::VK_K: ret = "VK_K"; break;
        case VirtualKeys::VK_L: ret = "VK_L"; break;
        case VirtualKeys::VK_M: ret = "VK_M"; break;
        case VirtualKeys::VK_N: ret = "VK_N"; break;
        case VirtualKeys::VK_O: ret = "VK_O"; break;
        case VirtualKeys::VK_P: ret = "VK_P"; break;
        case VirtualKeys::VK_Q: ret = "VK_Q"; break;
        case VirtualKeys::VK_R: ret = "VK_R"; break;
        case VirtualKeys::VK_S: ret = "VK_S"; break;
        case VirtualKeys::VK_T: ret = "VK_T"; break;
        case VirtualKeys::VK_U: ret = "VK_U"; break;
        case VirtualKeys::VK_V: ret = "VK_V"; break;
        case VirtualKeys::VK_W: ret = "VK_W"; break;
        case VirtualKeys::VK_X: ret = "VK_X"; break;
        case VirtualKeys::VK_Y: ret = "VK_Y"; break;
        case VirtualKeys::VK_Z: ret = "VK_Z"; break;
        case VirtualKeys::VK_LWIN: ret = "VK_LWIN"; break;
        case VirtualKeys::VK_RWIN: ret = "VK_RWIN"; break;
        case VirtualKeys::VK_APPS: ret = "VK_APPS"; break;
        case VirtualKeys::VK_SLEEP: ret = "VK_SLEEP"; break;
        case VirtualKeys::VK_MULTIPLY: ret = "VK_MULTIPLY"; break;
        case VirtualKeys::VK_ADD: ret = "VK_ADD"; break;
        case VirtualKeys::VK_SUBTRACT: ret = "VK_SUBTRACT"; break;
        case VirtualKeys::VK_DIVIDE: ret = "VK_DIVIDE"; break;
        case VirtualKeys::VK_F1: ret = "VK_F1"; break;
        case VirtualKeys::VK_F2: ret = "VK_F2"; break;
        case VirtualKeys::VK_F3: ret = "VK_F3"; break;
        case VirtualKeys::VK_F4: ret = "VK_F4"; break;
        case VirtualKeys::VK_F5: ret = "VK_F5"; break;
        case VirtualKeys::VK_F6: ret = "VK_F6"; break;
        case VirtualKeys::VK_F7: ret = "VK_F7"; break;
        case VirtualKeys::VK_F8: ret = "VK_F8"; break;
        case VirtualKeys::VK_F9: ret = "VK_F9"; break;
        case VirtualKeys::VK_F10: ret = "VK_F10"; break;
        case VirtualKeys::VK_F11: ret = "VK_F11"; break;
        case VirtualKeys::VK_F12: ret = "VK_F12"; break;
        case VirtualKeys::VK_F13: ret = "VK_F13"; break;
        case VirtualKeys::VK_F14: ret = "VK_F14"; break;
        case VirtualKeys::VK_F15: ret = "VK_F15"; break;
        case VirtualKeys::VK_F16: ret = "VK_F16"; break;
        case VirtualKeys::VK_F17: ret = "VK_F17"; break;
        case VirtualKeys::VK_F18: ret = "VK_F18"; break;
        case VirtualKeys::VK_F19: ret = "VK_F19"; break;
        case VirtualKeys::VK_F20: ret = "VK_F20"; break;
        case VirtualKeys::VK_F21: ret = "VK_F21"; break;
        case VirtualKeys::VK_F22: ret = "VK_F22"; break;
        case VirtualKeys::VK_F23: ret = "VK_F23"; break;
        case VirtualKeys::VK_F24: ret = "VK_F24"; break;
        case VirtualKeys::VK_NUMLOCK: ret = "VK_NUMLOCK"; break;
        case VirtualKeys::VK_SCROLL: ret = "VK_SCROLL"; break;
        case VirtualKeys::VK_LSHIFT: ret = "VK_LSHIFT"; break;
        case VirtualKeys::VK_RSHIFT: ret = "VK_RSHIFT"; break;
        case VirtualKeys::VK_LCONTROL: ret = "VK_LCONTROL"; break;
        case VirtualKeys::VK_RCONTROL: ret = "VK_RCONTROL"; break;
        case VirtualKeys::VK_LMENU: ret = "VK_LMENU"; break;
        case VirtualKeys::VK_RMENU: ret = "VK_RMENU"; break;
        case VirtualKeys::VK_BROWSER_BACK: ret = "VK_BROWSER_BACK"; break;
        case VirtualKeys::VK_BROWSER_FORWARD: ret = "VK_BROWSER_FORWARD"; break;
        case VirtualKeys::VK_BROWSER_REFRESH: ret = "VK_BROWSER_REFRESH"; break;
        case VirtualKeys::VK_BROWSER_STOP: ret = "VK_BROWSER_STOP"; break;
        case VirtualKeys::VK_BROWSER_SEARCH: ret = "VK_BROWSER_SEARCH"; break;
        case VirtualKeys::VK_BROWSER_FAVORITES: ret = "VK_BROWSER_FAVORITES"; break;
        case VirtualKeys::VK_BROWSER_HOME: ret = "VK_BROWSER_HOME"; break;
        case VirtualKeys::VK_VOLUME_MUTE: ret = "VK_VOLUME_MUTE"; break;
        case VirtualKeys::VK_VOLUME_DOWN: ret = "VK_VOLUME_DOWN"; break;
        case VirtualKeys::VK_VOLUME_UP: ret = "VK_VOLUME_UP"; break;
        case VirtualKeys::VK_MEDIA_NEXT_TRACK: ret = "VK_MEDIA_NEXT_TRACK"; break;
        case VirtualKeys::VK_MEDIA_PREV_TRACK: ret = "VK_MEDIA_PREV_TRACK"; break;
        case VirtualKeys::VK_MEDIA_STOP: ret = "VK_MEDIA_STOP"; break;
        case VirtualKeys::VK_MEDIA_PLAY_PAUSE: ret = "VK_MEDIA_PLAY_PAUSE"; break;
        case VirtualKeys::VK_LAUNCH_MAIL: ret = "VK_LAUNCH_MAIL"; break;
        case VirtualKeys::VK_LAUNCH_MEDIA_SELECT: ret = "VK_LAUNCH_MEDIA_SELECT"; break;
        case VirtualKeys::VK_LAUNCH_APP1: ret = "VK_LAUNCH_APP1"; break;
        case VirtualKeys::VK_LAUNCH_APP2: ret = "VK_LAUNCH_APP2"; break;
        case VirtualKeys::VK_OEM_1: ret = "VK_OEM_1"; break;
        case VirtualKeys::VK_OEM_PLUS: ret = "VK_OEM_PLUS"; break;
        case VirtualKeys::VK_OEM_COMMA: ret = "VK_OEM_COMMA"; break;
        case VirtualKeys::VK_OEM_MINUS: ret = "VK_OEM_MINUS"; break;
        case VirtualKeys::VK_OEM_PERIOD: ret = "VK_OEM_PERIOD"; break;
        case VirtualKeys::VK_OEM_2: ret = "VK_OEM_2"; break;
        case VirtualKeys::VK_OEM_3: ret = "VK_OEM_3"; break;
        case VirtualKeys::VK_ABNT_C1: ret = "VK_ABNT_C1"; break;
        case VirtualKeys::VK_ABNT_C2: ret = "VK_ABNT_C2"; break;
        case VirtualKeys::VK_OEM_4: ret = "VK_OEM_4"; break;
        case VirtualKeys::VK_OEM_5: ret = "VK_OEM_5"; break;
        case VirtualKeys::VK_OEM_6: ret = "VK_OEM_6"; break;
        case VirtualKeys::VK_OEM_7: ret = "VK_OEM_7"; break;
        default: ret = "Unknown"; break;
    }

    return ret;
}
