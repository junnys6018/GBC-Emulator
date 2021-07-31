#pragma once
#include "util/common.h"

namespace gbc
{
    class Divider
    {
    public:
        Divider(u32 period_log2) : m_period_log2(period_log2) {}
        inline u32 add_cycles(u32 cycles) const
        {
            m_counter += cycles;
            u32 ret = (m_counter >> m_period_log2);
            m_counter &= ~(0xFFFFFFFF << m_period_log2);
            return ret;
        }
        inline u32 get_counter() const { return m_counter; }

    public:
        u32 m_period_log2;

    private:
        mutable u32 m_counter = 0;
    };
}