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
#include "version-git.h"

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#define VERSION_MAJOR       2
#define VERSION_MINOR       0
#define VERSION_REVISION    0

#ifdef _DEBUG
#define VER_FILE_VERSION        VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define VER_FILE_VERSION_STR    STRINGIZE(VERSION_MAJOR) \
"." STRINGIZE(VERSION_MINOR) \
"." STRINGIZE(VERSION_REVISION) \
"." STRINGIZE(VERSION_BUILD)
#else
#define VER_FILE_VERSION        VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, 0
#define VER_FILE_VERSION_STR    STRINGIZE(VERSION_MAJOR) \
"." STRINGIZE(VERSION_MINOR) \
"." STRINGIZE(VERSION_REVISION)
#endif

#define VER_PRODUCT_VERSION     VER_FILE_VERSION
#define VER_PRODUCT_VERSION_STR VER_FILE_VERSION_STR

std::string GetAppVersion();
