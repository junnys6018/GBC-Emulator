#pragma once
#include "util/assert.h"
// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)
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

    extern bool s_silent;

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
                      std::std::is_trivially_constructible_v<To>);
        To dst;
        std::memcpy(&dst, &val, sizeof(To));
        return dst;
    }
#endif
}
