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
        case VirtualKeys::VKC_CANCEL: ret = "VKC_CANCEL"; break;
        case VirtualKeys::VKC_BACK: ret = "VKC_BACK"; break;
        case VirtualKeys::VKC_TAB: ret = "VKC_TAB"; break;
        case VirtualKeys::VKC_CLEAR: ret = "VKC_CLEAR"; break;
        case VirtualKeys::VKC_RETURN: ret = "VKC_RETURN"; break;
        case VirtualKeys::VKC_PAUSE: ret = "VKC_PAUSE"; break;
        case VirtualKeys::VKC_CAPITAL: ret = "VKC_CAPITAL"; break;
        case VirtualKeys::VKC_ESCAPE: ret = "VKC_ESCAPE"; break;
        case VirtualKeys::VKC_SPACE: ret = "VKC_SPACE"; break;
        case VirtualKeys::VKC_PRIOR: ret = "VKC_PRIOR"; break;
        case VirtualKeys::VKC_NEXT: ret = "VKC_NEXT"; break;
        case VirtualKeys::VKC_END: ret = "VKC_END"; break;
        case VirtualKeys::VKC_HOME: ret = "VKC_HOME"; break;
        case VirtualKeys::VKC_LEFT: ret = "VKC_LEFT"; break;
        case VirtualKeys::VKC_UP: ret = "VKC_UP"; break;
        case VirtualKeys::VKC_RIGHT: ret = "VKC_RIGHT"; break;
        case VirtualKeys::VKC_DOWN: ret = "VKC_DOWN"; break;
        case VirtualKeys::VKC_SNAPSHOT: ret = "VKC_SNAPSHOT"; break;
        case VirtualKeys::VKC_INSERT: ret = "VKC_INSERT"; break;
        case VirtualKeys::VKC_DELETE: ret = "VKC_DELETE"; break;
        case VirtualKeys::VKC_HELP: ret = "VKC_HELP"; break;
        case VirtualKeys::VKC_0: ret = "VKC_0"; break;
        case VirtualKeys::VKC_1: ret = "VKC_1"; break;
        case VirtualKeys::VKC_2: ret = "VKC_2"; break;
        case VirtualKeys::VKC_3: ret = "VKC_3"; break;
        case VirtualKeys::VKC_4: ret = "VKC_4"; break;
        case VirtualKeys::VKC_5: ret = "VKC_5"; break;
        case VirtualKeys::VKC_6: ret = "VKC_6"; break;
        case VirtualKeys::VKC_7: ret = "VKC_7"; break;
        case VirtualKeys::VKC_8: ret = "VKC_8"; break;
        case VirtualKeys::VKC_9: ret = "VKC_9"; break;
        case VirtualKeys::VKC_A: ret = "VKC_A"; break;
        case VirtualKeys::VKC_B: ret = "VKC_B"; break;
        case VirtualKeys::VKC_C: ret = "VKC_C"; break;
        case VirtualKeys::VKC_D: ret = "VKC_D"; break;
        case VirtualKeys::VKC_E: ret = "VKC_E"; break;
        case VirtualKeys::VKC_F: ret = "VKC_F"; break;
        case VirtualKeys::VKC_G: ret = "VKC_G"; break;
        case VirtualKeys::VKC_H: ret = "VKC_H"; break;
        case VirtualKeys::VKC_I: ret = "VKC_I"; break;
        case VirtualKeys::VKC_J: ret = "VKC_J"; break;
        case VirtualKeys::VKC_K: ret = "VKC_K"; break;
        case VirtualKeys::VKC_L: ret = "VKC_L"; break;
        case VirtualKeys::VKC_M: ret = "VKC_M"; break;
        case VirtualKeys::VKC_N: ret = "VKC_N"; break;
        case VirtualKeys::VKC_O: ret = "VKC_O"; break;
        case VirtualKeys::VKC_P: ret = "VKC_P"; break;
        case VirtualKeys::VKC_Q: ret = "VKC_Q"; break;
        case VirtualKeys::VKC_R: ret = "VKC_R"; break;
        case VirtualKeys::VKC_S: ret = "VKC_S"; break;
        case VirtualKeys::VKC_T: ret = "VKC_T"; break;
        case VirtualKeys::VKC_U: ret = "VKC_U"; break;
        case VirtualKeys::VKC_V: ret = "VKC_V"; break;
        case VirtualKeys::VKC_W: ret = "VKC_W"; break;
        case VirtualKeys::VKC_X: ret = "VKC_X"; break;
        case VirtualKeys::VKC_Y: ret = "VKC_Y"; break;
        case VirtualKeys::VKC_Z: ret = "VKC_Z"; break;
        case VirtualKeys::VKC_LWIN: ret = "VKC_LWIN"; break;
        case VirtualKeys::VKC_RWIN: ret = "VKC_RWIN"; break;
        case VirtualKeys::VKC_APPS: ret = "VKC_APPS"; break;
        case VirtualKeys::VKC_SLEEP: ret = "VKC_SLEEP"; break;
        case VirtualKeys::VKC_MULTIPLY: ret = "VKC_MULTIPLY"; break;
        case VirtualKeys::VKC_ADD: ret = "VKC_ADD"; break;
        case VirtualKeys::VKC_SUBTRACT: ret = "VKC_SUBTRACT"; break;
        case VirtualKeys::VKC_DIVIDE: ret = "VKC_DIVIDE"; break;
        case VirtualKeys::VKC_F1: ret = "VKC_F1"; break;
        case VirtualKeys::VKC_F2: ret = "VKC_F2"; break;
        case VirtualKeys::VKC_F3: ret = "VKC_F3"; break;
        case VirtualKeys::VKC_F4: ret = "VKC_F4"; break;
        case VirtualKeys::VKC_F5: ret = "VKC_F5"; break;
        case VirtualKeys::VKC_F6: ret = "VKC_F6"; break;
        case VirtualKeys::VKC_F7: ret = "VKC_F7"; break;
        case VirtualKeys::VKC_F8: ret = "VKC_F8"; break;
        case VirtualKeys::VKC_F9: ret = "VKC_F9"; break;
        case VirtualKeys::VKC_F10: ret = "VKC_F10"; break;
        case VirtualKeys::VKC_F11: ret = "VKC_F11"; break;
        case VirtualKeys::VKC_F12: ret = "VKC_F12"; break;
        case VirtualKeys::VKC_F13: ret = "VKC_F13"; break;
        case VirtualKeys::VKC_F14: ret = "VKC_F14"; break;
        case VirtualKeys::VKC_F15: ret = "VKC_F15"; break;
        case VirtualKeys::VKC_F16: ret = "VKC_F16"; break;
        case VirtualKeys::VKC_F17: ret = "VKC_F17"; break;
        case VirtualKeys::VKC_F18: ret = "VKC_F18"; break;
        case VirtualKeys::VKC_F19: ret = "VKC_F19"; break;
        case VirtualKeys::VKC_F20: ret = "VKC_F20"; break;
        case VirtualKeys::VKC_F21: ret = "VKC_F21"; break;
        case VirtualKeys::VKC_F22: ret = "VKC_F22"; break;
        case VirtualKeys::VKC_F23: ret = "VKC_F23"; break;
        case VirtualKeys::VKC_F24: ret = "VKC_F24"; break;
        case VirtualKeys::VKC_NUMLOCK: ret = "VKC_NUMLOCK"; break;
        case VirtualKeys::VKC_SCROLL: ret = "VKC_SCROLL"; break;
        case VirtualKeys::VKC_LSHIFT: ret = "VKC_LSHIFT"; break;
        case VirtualKeys::VKC_RSHIFT: ret = "VKC_RSHIFT"; break;
        case VirtualKeys::VKC_LCONTROL: ret = "VKC_LCONTROL"; break;
        case VirtualKeys::VKC_RCONTROL: ret = "VKC_RCONTROL"; break;
        case VirtualKeys::VKC_LMENU: ret = "VKC_LMENU"; break;
        case VirtualKeys::VKC_RMENU: ret = "VKC_RMENU"; break;
        case VirtualKeys::VKC_BROWSER_BACK: ret = "VKC_BROWSER_BACK"; break;
        case VirtualKeys::VKC_BROWSER_FORWARD: ret = "VKC_BROWSER_FORWARD"; break;
        case VirtualKeys::VKC_BROWSER_REFRESH: ret = "VKC_BROWSER_REFRESH"; break;
        case VirtualKeys::VKC_BROWSER_STOP: ret = "VKC_BROWSER_STOP"; break;
        case VirtualKeys::VKC_BROWSER_SEARCH: ret = "VKC_BROWSER_SEARCH"; break;
        case VirtualKeys::VKC_BROWSER_FAVORITES: ret = "VKC_BROWSER_FAVORITES"; break;
        case VirtualKeys::VKC_BROWSER_HOME: ret = "VKC_BROWSER_HOME"; break;
        case VirtualKeys::VKC_VOLUME_MUTE: ret = "VKC_VOLUME_MUTE"; break;
        case VirtualKeys::VKC_VOLUME_DOWN: ret = "VKC_VOLUME_DOWN"; break;
        case VirtualKeys::VKC_VOLUME_UP: ret = "VKC_VOLUME_UP"; break;
        case VirtualKeys::VKC_MEDIA_NEXT_TRACK: ret = "VKC_MEDIA_NEXT_TRACK"; break;
        case VirtualKeys::VKC_MEDIA_PREV_TRACK: ret = "VKC_MEDIA_PREV_TRACK"; break;
        case VirtualKeys::VKC_MEDIA_STOP: ret = "VKC_MEDIA_STOP"; break;
        case VirtualKeys::VKC_MEDIA_PLAY_PAUSE: ret = "VKC_MEDIA_PLAY_PAUSE"; break;
        case VirtualKeys::VKC_LAUNCH_MAIL: ret = "VKC_LAUNCH_MAIL"; break;
        case VirtualKeys::VKC_LAUNCH_MEDIA_SELECT: ret = "VKC_LAUNCH_MEDIA_SELECT"; break;
        case VirtualKeys::VKC_LAUNCH_APP1: ret = "VKC_LAUNCH_APP1"; break;
        case VirtualKeys::VKC_LAUNCH_APP2: ret = "VKC_LAUNCH_APP2"; break;
        case VirtualKeys::VKC_OEM_1: ret = "VKC_OEM_1"; break;
        case VirtualKeys::VKC_OEM_PLUS: ret = "VKC_OEM_PLUS"; break;
        case VirtualKeys::VKC_OEM_COMMA: ret = "VKC_OEM_COMMA"; break;
        case VirtualKeys::VKC_OEM_MINUS: ret = "VKC_OEM_MINUS"; break;
        case VirtualKeys::VKC_OEM_PERIOD: ret = "VKC_OEM_PERIOD"; break;
        case VirtualKeys::VKC_OEM_2: ret = "VKC_OEM_2"; break;
        case VirtualKeys::VKC_OEM_3: ret = "VKC_OEM_3"; break;
        case VirtualKeys::VKC_ABNT_C1: ret = "VKC_ABNT_C1"; break;
        case VirtualKeys::VKC_ABNT_C2: ret = "VKC_ABNT_C2"; break;
        case VirtualKeys::VKC_OEM_4: ret = "VKC_OEM_4"; break;
        case VirtualKeys::VKC_OEM_5: ret = "VKC_OEM_5"; break;
        case VirtualKeys::VKC_OEM_6: ret = "VKC_OEM_6"; break;
        case VirtualKeys::VKC_OEM_7: ret = "VKC_OEM_7"; break;
        default: ret = "Unknown"; break;
    }

    return ret;
}
