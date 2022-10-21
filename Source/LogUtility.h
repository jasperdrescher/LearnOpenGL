#pragma once

#include <cstdarg>

namespace LogUtility
{
    enum class LogCategory { Core, File, GLFW, GLAD, GL, Graphics };
    static const char* LogCategoryToChar(LogCategory aLogCategory)
    {
        switch (aLogCategory)
        {
            case LogCategory::Core: return "Core";
            case LogCategory::File: return "File";
            case LogCategory::GLFW: return "GLFW";
            case LogCategory::GLAD: return "GLAD";
            case LogCategory::GL: return "GL";
            case LogCategory::Graphics: return "Graphics";
        }

        return "N/A";
    }

    static void PrintMessage(LogCategory aCategory, const char* aMessageFormat...)
    {
        char buffer[256];
        va_list arguments;
        va_start(arguments, aMessageFormat);
        const int length = std::vsnprintf(buffer, sizeof buffer, aMessageFormat, arguments);
        va_end(arguments);

        if (length > 0)
            std::printf("[Debug] [%s] %s\n", LogCategoryToChar(aCategory), buffer);
    }

    static void PrintError(LogCategory aCategory, const char* aMessageFormat...)
    {
        char buffer[256];
        va_list arguments;
        va_start(arguments, aMessageFormat);
        const int length = std::vsnprintf(buffer, sizeof buffer, aMessageFormat, arguments);
        va_end(arguments);

        if (length > 0)
            std::printf("[Error] [%s] %s\n", LogCategoryToChar(aCategory), buffer);
    }
}
