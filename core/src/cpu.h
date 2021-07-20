#pragma once
#include "bus.h"
#include "util/common.h"

namespace gbc
{
    constexpr i32 REG_B = 0;
    constexpr i32 REG_C = 1;
    constexpr i32 REG_D = 2;
    constexpr i32 REG_E = 3;
    constexpr i32 REG_H = 4;
    constexpr i32 REG_L = 5;
    constexpr i32 REG_IND_HL = 6;
    constexpr i32 REG_A = 7;
    constexpr i32 REG_IND_BC = 8;
    constexpr i32 REG_IND_DE = 9;
    constexpr i32 REG_IND_HL_INC = 10;
    constexpr i32 REG_IND_HL_DEC = 11;
    constexpr i32 NUM_REG = 12;

    struct Register
    {
        u8 low;
        u8 high;
        inline u16 get()
        {
            return (static_cast<u16>(high) << 8) | static_cast<u16>(low);
        }
        inline void set(u16 value)
        {
            low = value & 0x00FF;
            high = (value & 0xFF00) >> 8;
        }
    };

    class CPU
    {
    public:
        CPU(const Ref<Bus>& bus);
        void clock();
        void run_until(u64 clock);

    private:
        struct
        {
            u8 unused : 4;
            u8 z : 1; // zero flag
            u8 n : 1; // subtraction flag
            u8 h : 1; // half carry flag
            u8 c : 1; // carry flag
            u8 acc;   // accumulator
        } AF;
        Register BC, DE, HL;
        u16 SP, PC;

        u64 m_num_m_cycles;
        Ref<Bus> m_bus;

    private:
        template <i32 R>
        inline u8 get_register()
        {
            static_assert(R < NUM_REG);

            if constexpr (R == REG_B)
                return BC.high;
            if constexpr (R == REG_C)
                return BC.low;
            if constexpr (R == REG_D)
                return DE.high;
            if constexpr (R == REG_E)
                return DE.low;
            if constexpr (R == REG_H)
                return HL.high;
            if constexpr (R == REG_L)
                return HL.low;
            if constexpr (R == REG_IND_HL)
                return m_bus->cpu_read_byte(HL.get());
            if constexpr (R == REG_A)
                return AF.acc;
            if constexpr (R == REG_IND_BC)
                return m_bus->cpu_read_byte(BC.get());
            if constexpr (R == REG_IND_DE)
                return m_bus->cpu_read_byte(DE.get());
            if constexpr (R == REG_IND_HL_INC)
            {
                u16 addr = HL.get();
                HL.set(addr + 1);
                return m_bus->cpu_read_byte(addr);
            }
            if constexpr (R == REG_IND_HL_DEC)
            {
                u16 addr = HL.get();
                HL.set(addr - 1);
                return m_bus->cpu_read_byte(addr);
            }
        }
        template <i32 R>
        inline void write_register(u8 data)
        {
            static_assert(R < NUM_REG);

            if constexpr (R == REG_B)
                BC.high = data;
            if constexpr (R == REG_C)
                BC.low = data;
            if constexpr (R == REG_D)
                DE.high = data;
            if constexpr (R == REG_E)
                DE.low = data;
            if constexpr (R == REG_H)
                HL.high = data;
            if constexpr (R == REG_L)
                HL.low = data;
            if constexpr (R == REG_IND_HL)
                m_bus->cpu_write_byte(HL.get(), data);
            if constexpr (R == REG_A)
                AF.acc = data;
            if constexpr (R == REG_IND_BC)
                return m_bus->cpu_write_byte(BC.get(), data);
            if constexpr (R == REG_IND_DE)
                return m_bus->cpu_write_byte(DE.get(), data);
            if constexpr (R == REG_IND_HL_INC)
            {
                u16 addr = HL.get();
                HL.set(addr + 1);
                return m_bus->cpu_write_byte(addr, data);
            }
            if constexpr (R == REG_IND_HL_DEC)
            {
                u16 addr = HL.get();
                HL.set(addr - 1);
                return m_bus->cpu_write_byte(addr, data);
            }
        }

        template <i32 R>
        inline u16 get_register_16()
        {
            static_assert(R < 4);

            if constexpr (R == 0)
                return BC.get();
            if constexpr (R == 1)
                return DE.get();
            if constexpr (R == 2)
                return HL.get();
            if constexpr (R == 3)
                return SP;
        }

        template <i32 R>
        inline void write_register_16(u16 data)
        {
            static_assert(R < 4);

            if constexpr (R == 0)
                BC.set(data);
            if constexpr (R == 1)
                DE.set(data);
            if constexpr (R == 2)
                HL.set(data);
            if constexpr (R == 3)
                SP = data;
        }

        template <i32 Ri, i32 Ro>
        void ld()
        {
            u8 data = get_register<Ri>();
            write_register<Ro>(data);
        }

        template <i32 R>
        void ld_imm()
        {
            u8 data = m_bus->cpu_read_byte(PC++);
            write_register<R>(data);
        }

        template <i32 R>
        void ld_imm_16()
        {
            u16 low = m_bus->cpu_read_byte(PC++);
            u16 high = m_bus->cpu_read_byte(PC++);
            u16 data = (high << 8) | low;
            write_register_16<R>(data);
        }

        template <i32 R>
        void add()
        {
            u16 data = get_register<R>();
            u16 result = static_cast<u16>(AF.acc) + data;

            // Store result into acc
            AF.acc = result & 0x00FF;

            // Flags
            AF.z = (AF.acc == 0);
            AF.n = 0;
            AF.h = ((((data & 0x0F) + (AF.acc & 0x0F)) & 0x10) == 0x10);
            AF.c = (result > 0xFF);
        }

        template <i32 R>
        void adc()
        {
            u16 data = get_register<R>();
            u16 result = static_cast<u16>(AF.acc) + data + AF.c;

            // Store result into acc
            AF.acc = result & 0x00FF;

            // Flags
            AF.z = (AF.acc == 0);
            AF.n = 0;
            AF.h = ((((data & 0x0F) + (AF.acc & 0x0F) + AF.c) & 0x10) == 0x10);
            AF.c = (result > 0xFF);
        }

        template <i32 R>
        void sub()
        {
            u16 data = get_register<R>();
            u16 result = static_cast<u16>(AF.acc) + (data ^ 0x00FF) + 1;

            // Store result into acc
            AF.acc = result & 0x00FF;

            // Flags
            AF.z = (AF.acc == 0);
            AF.n = 1;
            AF.h = !(((((data & 0x0F) ^ 0x0F) + 1 + (AF.acc & 0x0F)) & 0x10) == 0x10);
            AF.c = !(result > 0xFF);
        }

        template <i32 R>
        void sbc()
        {
            u16 data = get_register<R>();
            u16 result = static_cast<u16>(AF.acc) + ((data + AF.c) ^ 0x00FF);

            // Store result into acc
            AF.acc = result & 0x00FF;

            // Flags
            AF.z = (AF.acc == 0);
            AF.n = 1;
            AF.h = !(((((data & 0x0F) ^ 0x0F) + 1 + (AF.acc & 0x0F)) & 0x10) == 0x10);
            AF.c = !(result > 0xFF);
        }
        // clang-format off
        template <i32 R>
        void and()
        {
            AF.acc &= get_register<R>();

            // Flags
            AF.z = (AF.acc == 0);
            AF.n = 0;
            AF.h = 1;
            AF.c = 0;
        }

        template <i32 R>
        void xor()
        {
            AF.acc ^= get_register<R>();

            // Flags
            AF.z = (AF.acc == 0);
            AF.n = 0;
            AF.h = 0;
            AF.c = 0;
        }

        template <i32 R>
        void or()
        {
            AF.acc |= get_register<R>();

            // Flags
            AF.z = (AF.acc == 0);
            AF.n = 0;
            AF.h = 0;
            AF.c = 0;
        }
        // clang-format on
        template <i32 R>
        void cp()
        {
            u16 data = get_register<R>();
            u16 result = static_cast<u16>(AF.acc) + (data ^ 0x00FF) + 1;

            // Flags
            AF.z = (result & 0x00FF) == 0;
            AF.n = 1;
            AF.h = !(((((data & 0x0F) ^ 0x0F) + 1 + (AF.acc & 0x0F)) & 0x10) == 0x10);
            AF.c = !(result > 0xFF);
        }

        template <i32 R>
        void inc()
        {
            u8 reg = get_register<R>();
            write_register<R>(reg + 1);

            // Flags
            AF.z = ((reg + 1) == 0);
            AF.n = 0;
            AF.h = (((reg & 0x0F) + 1) & 0x10) == 0x10;
        }

        template <i32 R>
        void dec()
        {
            u8 reg = get_register<R>();
            write_register<R>(reg - 1);

            // Flags
            AF.z = ((reg - 1) == 0);
            AF.n = 1;
            AF.h = (reg & 0x0F) == 0;
        }

        template <i32 R>
        void inc_16()
        {
            u16 reg = get_register_16<R>();
            write_register_16<R>(reg + 1);
        }

        template <i32 R>
        void dec_16()
        {
            u16 reg = get_register_16<R>();
            write_register_16<R>(reg - 1);
        }
    };
}