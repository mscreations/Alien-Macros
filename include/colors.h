#pragma once
#include <string>
#include <vector>

enum class Colors
{
    Black = 30,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    BrightBlack = 90,
    BrightRed,
    BrightGreen,
    BrightYellow,
    BrightBlue,
    BrightMagenta,
    BrightCyan,
    BrightWhite

};

enum class Effects
{
    Reset,
    Bold,
    Faint,
    Italic,
    Underline
};

std::string ResetColors();
std::string MakeColorCode(Colors fg, Colors bg = Colors::Black, const std::vector<Effects>& effects = {});