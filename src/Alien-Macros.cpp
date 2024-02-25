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

#include <iostream>
#include "version.h"
#include "ProgSettings.h"
#include "HidDevice.h"
#include "MacroHandler.h"

int main()
{
    std::cout << "Alien Macros - Version " << GetAppVersion() << std::endl;

    ProgSettings ps{};
    MacroHandler mh(ps.getMacros());

    HidDevicePtr device = ps.getDevice();

    device->Open(true);
    while (true)
    {
        device.get()->Read();
        unsigned short key = device.get()->getKeyPress();
        if (key == 0x00) { continue; }
        std::cout << "Read: 0x" << std::hex << key << " " << ps.getDescription(key) << std::endl;
        mh.Process(key);
    }
}