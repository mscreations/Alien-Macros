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

#pragma once

#include "hid.h"
#include <minwindef.h>

// Following are correct for Alienware m17 R4. Other machines may need other VID/PIDs. Problem for another day.
#define AW_KB_VID       "0x0d62"
#define AW_KB_PID       "0x1a1c"
#define AW_USAGEPAGE    0x0c
#define AW_USAGE        0x01

#define MACROA          0x4c
#define MACROB          0x4d
#define MACROC          0x4e
#define MACROD          0x4f

#define READ_THREAD_TIMEOUT     1000

bool StartMonitor(WORD targetVID, WORD targetPID);
