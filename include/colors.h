#pragma once

namespace Colors
{
    constexpr auto ClearScreen = "\033[2J";
    constexpr auto CursorPreviousLine = "\033[F\033[J";
    constexpr auto Reset = "\033[0m";
    constexpr auto Red = "\033[31m";
    constexpr auto Green = "\033[32m";
    constexpr auto Yellow = "\033[33m";
    constexpr auto Blue = "\033[34m";
    constexpr auto Magenta = "\033[35m";
    constexpr auto Cyan = "\033[36m";
    constexpr auto White = "\033[37m";
    constexpr auto BrightBlack = "\033[90m";
    constexpr auto BrightRed = "\033[91m";
    constexpr auto BrightGreen = "\033[92m";
    constexpr auto BrightYellow = "\033[93m";
    constexpr auto BrightBlue = "\033[94m";
    constexpr auto BrightMagenta = "\033[95m";
    constexpr auto BrightCyan = "\033[96m";
    constexpr auto BrightWhite = "\033[97m";
}
