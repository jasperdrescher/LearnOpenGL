#pragma once

#include <cstdarg>

namespace LogUtility
{
    static void PrintMessage(const char* aMessageFormat...)
    {
        char buffer[256];
        va_list arguments;
        va_start(arguments, aMessageFormat);
        const int length = std::vsnprintf(buffer, sizeof buffer, aMessageFormat, arguments);
        va_end(arguments);

        if (length > 0)
            std::printf("[Debug] %s\n", buffer);
    }

    static void PrintError(const char* aMessageFormat...)
    {
        char buffer[256];
        va_list arguments;
        va_start(arguments, aMessageFormat);
        const int length = std::vsnprintf(buffer, sizeof buffer, aMessageFormat, arguments);
        va_end(arguments);

        if (length > 0)
            std::printf("[Error] %s\n", buffer);
    }
}
