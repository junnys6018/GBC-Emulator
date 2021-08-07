#pragma once
#include <gbc.h>

inline std::string get_filename(const std::string& path)
{
    size_t backslash = path.find_last_of('/');
    if (backslash == std::string::npos)
        return path;
    return path.substr(backslash + 1);
}
