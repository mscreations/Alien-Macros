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

int main(int argc, char* argv[])
{

    ProgSettings ps{ argc, argv };

    std::cout << "Alien Macros - Version " << GetAppVersion() << std::endl;

#ifdef _DEBUG
    std::cout << ps << std::endl;
#endif

    HidDevices devices{};
    devices.FindAllHidDevices();

    MacroHandler mh{};

    std::vector<HidDevicePtr>& list = devices.getDevices();

    for (auto& dev : list)
    {
        if (dev->IsTarget(ps.getVID(), ps.getPID(), ps.getUsagePage(), ps.getUsageCode()))
        {
            std::cout << "Found target device: \n" << *(dev.get()) << std::endl;
            while (true)
            {
                dev.get()->Open(true);      // Open with Read Access. All other parameters default false.
                dev.get()->Read();

                std::cout << "Read: " << std::hex << dev.get()->getKeyPress() << std::endl;

                mh.Process(dev.get()->getKeyPress());
            }
        }
    }
}