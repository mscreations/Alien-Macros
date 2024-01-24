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

int main(int argc, const char* argv[])
{

    ProgSettings ps{ argc, argv };
    MacroHandler mh(ps.getMacros());

    std::cout << "Alien Macros - Version " << GetAppVersion() << std::endl;

#ifdef _DEBUG
    std::cout << ps << std::endl;
#endif

    HidDevices devices{};
    devices.FindAllHidDevices();

    const std::vector<HidDevicePtr>& list = devices.getDevices();

    for (const auto& dev : list)
    {
        if (dev->IsTarget(ps.getVID(), ps.getPID(), ps.getUsagePage(), ps.getUsageCode()))
        {
            std::cout << "Found target device: \n" << *(dev.get()) << std::endl;

            dev.get()->Open(true);      // Open with Read Access. All other parameters default false.
            while (true)
            {
                dev.get()->Read();

                unsigned short key = dev.get()->getKeyPress();

                if (key == 0x00) { continue; }

                std::cout << "Read: 0x" << std::hex << key << " " << ps.getDescription(key) << std::endl;

                mh.Process(key);
            }
        }
    }
}