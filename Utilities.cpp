#include "Utilities.h"
#include <Windows.h>

#pragma comment(lib,"Version.lib")

/*
 * This function is copied from the alienfx-tools project (https://github.com/T-Troll/alienfx-tools). The following license is applicable to that project:
 *
 * MIT License
 *
 * Copyright (c) 2020 Rik Lain
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
std::string GetAppVersion()
{

    HMODULE hInst = GetModuleHandle(NULL);

    HRSRC hResInfo = FindResource(hInst, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);

    std::string res;

    if (hResInfo)
    {
        DWORD dwSize = SizeofResource(hInst, hResInfo);
        HGLOBAL hResData = LoadResource(hInst, hResInfo);
        if (hResData)
        {
            LPVOID pRes = LockResource(hResData),
                pResCopy = LocalAlloc(LMEM_FIXED, dwSize);
            if (pResCopy)
            {
                UINT uLen = 0;
                VS_FIXEDFILEINFO* lpFfi = NULL;

                CopyMemory(pResCopy, pRes, dwSize);

                VerQueryValue(pResCopy, TEXT("\\"), reinterpret_cast<LPVOID*>(&lpFfi), &uLen);

                res = std::to_string(HIWORD(lpFfi->dwProductVersionMS)) + "."
                    + std::to_string(LOWORD(lpFfi->dwProductVersionMS)) + "."
                    + std::to_string(HIWORD(lpFfi->dwProductVersionLS)) + "."
                    + std::to_string(LOWORD(lpFfi->dwProductVersionLS));

                LocalFree(pResCopy);
            }
            FreeResource(hResData);
        }
    }
    return res;
}