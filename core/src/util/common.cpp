#include "util/common.h"
#include "util/log.h"
#include <fstream>
namespace gbc
{
    const u32 s_dmg_palette[4] = {COL32(155, 188, 15, 255), COL32(139, 172, 15, 255), COL32(48, 98, 48, 255), COL32(15, 56, 15, 255)};
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

    bool write_file(const std::string& filename, const std::vector<u8>& bytes)
    {
        std::ofstream outfile(filename, std::ios::binary);
        if (outfile.fail())
            return false;

        outfile.write((const char*)bytes.data(), bytes.size());
        return true;
    }

}