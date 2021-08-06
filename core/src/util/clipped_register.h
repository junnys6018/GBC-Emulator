#pragma once
#include "util/common.h"

namespace gbc
{

    template <typename T, u32 bits>
    class ClippedUnsigned
    {
    public:
        static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);
        static_assert(bits <= sizeof(T) * 8);
        ClippedUnsigned(T value = 0) : m_value(value & mask(bits)) {}
        inline void set(T value)
        {
            constexpr T m = mask(bits);
            m_value = (value & m);
        }
        inline T get() const { return m_value; }

    private:
        constexpr static u64 mask(u32 n)
        {
            u64 ret = 0;
            for (u32 i = 0; i < n; i++)
                ret |= ((u64)1 << i);

            return ret;
        }
        T m_value;
    };

    template <u32 bits>
    using ClippedU8 = ClippedUnsigned<u8, bits>;
    template <u32 bits>
    using ClippedU16 = ClippedUnsigned<u16, bits>;
    template <u32 bits>
    using ClippedU32 = ClippedUnsigned<u32, bits>;
    template <u32 bits>
    using ClippedU64 = ClippedUnsigned<u64, bits>;
}