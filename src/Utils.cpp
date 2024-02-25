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
#include <iostream>
#include <format>
#include <sstream>
#include <algorithm>
#include "colors.h"

void Utils::ResetCin()
{
    // Check if input buffer has anything in it
    if (std::cin.rdbuf()->in_avail() != 0)
    {
        // if it is, ignore it
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

bool Utils::ConvertToNumeric(const std::string& userInput, unsigned int& response)
{
    response = UINT_MAX;

    if (userInput.empty()) { return false; }

    // Ensure all characters in string are numeric values
    if (!std::all_of(userInput.begin(), userInput.end(), [](char c) { return std::isdigit(c); })) { return false; }

    try
    {
        response = std::stoi(userInput);
    }
    catch (const std::invalid_argument& ex)
    {
        std::cerr << "Invalid argument: " << ex.what() << std::endl << std::endl;
        return false;
    }
    catch (const std::out_of_range& ex)
    {
        std::cerr << "Out of range: " << ex.what() << std::endl << std::endl;
        return false;
    }

    return true;
}

/// <summary>
/// Shows user a question and prompts for a response.
/// </summary>
/// <param name="message"></param>
/// <param name="response"></param>
/// <returns></returns>
bool Utils::AskResponse(const std::string& message, unsigned int& response, const unsigned int maxValue)
{
    std::string userInput{};
    std::cout << std::endl;

    do
    {
        ResetCin();
        std::cout << Colors::CursorPreviousLine << std::format("{}: ", message);
        std::getline(std::cin, userInput);

    } while (!ConvertToNumeric(userInput, response));

    if (response == 0 || response > maxValue)
    {
        response = UINT_MAX;
        std::cout << Colors::CursorPreviousLine;
        return false;
    }
    return true;
}

/// <summary>
/// Shows user a question with a range of valid entries and prompts for valid response.
/// </summary>
/// <param name="message">Message to be displayed</param>
/// <param name="firstOption">First valid option</param>
/// <param name="lastOption">Last valid option</param>
/// <param name="response">Response value entered by user or UINT_MAX</param>
/// <returns>true if response is within valid range / false otherwise</returns>
bool Utils::AskResponse(const std::string& message, const unsigned int firstOption, const unsigned int lastOption, unsigned int& response)
{
    std::ostringstream oss;
    oss << std::format("{} [{} - {}]", message, firstOption, lastOption);
    return AskResponse(oss.str(), response, lastOption);
}

/// <summary>
/// Shows user a yes/no question and prompts for response. 
/// </summary>
/// <param name="message">Message to be displayed</param>
/// <param name="yesDefault">If true, pressing enter will equate to entering yes</param>
/// <returns>true if response is yes / false if response is no</returns>
bool Utils::AskResponse(const std::string& message, bool yesDefault)
{
    char response;
    do
    {
        ResetCin();
        std::cout << std::format("{} [{}/{}]: ", message, (yesDefault ? "Y" : "y"), (yesDefault ? "n" : "N"));
        std::cin.get(response);

        if (response == 'y' || response == 'Y' || (yesDefault && response == '\n'))
        {
            return true;
        }
        else if (response == 'n' || response == 'N')
        {
            return false;
        }
        else
        {
            std::cout << Colors::CursorPreviousLine;
            response = 0;
        }
    } while (!std::isalpha(response));
    return false;
}

/// <summary>
/// Shows user a question and prompts for a string response
/// </summary>
/// <param name="message">Message to display to user</param>
/// <param name="response">Response from user. Not empty.</param>
/// <returns>true if user provided a string / false if any issues or empty string</returns>
bool Utils::AskResponse(const std::string& message, std::string& response)
{
    ResetCin();

    std::cout << std::format("{}: ", message);
    std::getline(std::cin, response);

    if (response.empty()) { return false; }
    return true;
}

/// <summary>
/// Shows user  question and prompts for a character response
/// </summary>
/// <param name="message">Message to be displayed to user</param>
/// <param name="response">Response from empty. Checked that character is printable</param>
/// <returns>true if user provided char / false otherwise</returns>
bool Utils::AskResponse(const std::string& message, char& response)
{
    ResetCin();

    std::cout << std::format("{}: ", message);
    std::cin.get(response);

    if (!std::isprint(response)) { return false; }

    ResetCin();

    return true;
}

