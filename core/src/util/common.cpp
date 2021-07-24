#include "util/common.h"
#include "util/log.h"
#include <fstream>
namespace gbc
{
    bool s_silent = false;
    void initialize(spdlog::level::level_enum log_level, bool silent)
    {
        Log::initialize(log_level);
        s_silent = silent;
    }

    std::vector<u8> read_file(const std::string& filename)
    {
        std::ifstream infile(filename, std::ios::binary);
        std::vector<u8> data((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
        return data;
    }
}