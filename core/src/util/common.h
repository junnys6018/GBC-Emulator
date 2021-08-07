#pragma once
#include "util/assert.h"
// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

#if defined(GBC_COMPILE_TESTS)
class GBCTests;
#endif

#define COL32(R, G, B, A) (((u32)(A) << 24) | ((u32)(B) << 16) | ((u32)(G) << 8) | ((u32)(R) << 0))

namespace gbc
{
    using i8 = int8_t;
    using u8 = uint8_t;
    using i16 = int16_t;
    using u16 = uint16_t;
    using i32 = int32_t;
    using u32 = uint32_t;
    using i64 = int64_t;
    using u64 = uint64_t;

#if !defined(PLATFORM_WINDOWS) && !defined(PLATFORM_LINUX)
#error Unknown platform
#endif

#if !defined(CONFIGURATION_DEBUG) && !defined(CONFIGURATION_RELEASE)
#error Unknown configuration
#endif

    constexpr u32 MASTER_CLOCK_FREQ = 4194304;

    struct Keys
    {
        u8 right : 1;
        u8 left : 1;
        u8 up : 1;
        u8 down : 1;
        u8 a : 1;
        u8 b : 1;
        u8 select : 1;
        u8 start : 1;
    };

    enum class GBCMode
    {
        GBC,
        COMPATIBILITY
    };

    extern bool s_silent;
    extern const u32 s_dmg_palette[4];

    void initialize(spdlog::level::level_enum log_level = spdlog::level::trace, bool silent = false);

    template <typename T>
    using Scope = std::unique_ptr<T>;

    template <typename T, typename... Args>
    constexpr Scope<T> create_scope(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using Ref = std::shared_ptr<T>;

    template <typename T, typename... Args>
    constexpr Ref<T> create_ref(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    constexpr inline u8 bit(T data, i32 digit)
    {
        return (data & (1 << digit)) != 0;
    }

    inline u8 msb(u16 data) { return (data & 0xFF00) >> 8; }

    inline u8 lsb(u16 data) { return data & 0x00FF; }

    std::vector<u8> read_file(const std::string& filename);
    bool write_file(const std::string& filename, const std::vector<u8>& bytes);

    inline u32 bank(u32 bank, u32 bank_size) { return bank * bank_size; }

    /*
        calculates the mininum between two numbers a and b
        however if either a or b is equal to zero then the other
        number is returned
    */
    inline u32 clamped_min(u32 a, u32 b)
    {
        if (a == 0)
            return b;
        if (b == 0)
            return a;
        return a < b ? a : b;
    }

#ifdef GBC_HAS_CXX_20
    template <class To, class From>
    constexpr To bit_cast(const From& val) noexcept
    {
        static_assert((sizeof(To) == sizeof(From)) && std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From>);
        return std::bit_cast<To>(val);
    }
#else
    template <class To, class From>
    constexpr To bit_cast(const From& val) noexcept
    {
        // We additionally require the destination type to be trivially constructable
        static_assert((sizeof(To) == sizeof(From)) && std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From> &&
                      std::is_trivially_constructible_v<To>);
        To dst;
        std::memcpy(&dst, &val, sizeof(To));
        return dst;
    }
#endif
}
