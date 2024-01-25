#include <sstream>
#include "colors.h"

std::string ResetColors()
{
    return "\033[0m";
}

std::string MakeColorCode(Colors fg, Colors bg, const std::vector<Effects>& effects)
{
    std::ostringstream oss;

    // Set foreground color
    oss << "\033[" << static_cast<int>(fg);

    // Set background color
    oss << ";" << static_cast<int>(bg) + 10;

    // Add effects
    for (const auto& effect : effects)
    {
        oss << ";" << static_cast<int>(effect);
    }
    // Add terminator
    oss << "m";

    return oss.str();
}

