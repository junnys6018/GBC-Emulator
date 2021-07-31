#include "cpu.h"

#include "util/log.h"

#define SET_REG_16()                                                                                                                       \
    static_assert(R < 4);                                                                                                                  \
    if constexpr (R == 0)                                                                                                                  \
        BC.set(data);                                                                                                                      \
    if constexpr (R == 1)                                                                                                                  \
        DE.set(data);                                                                                                                      \
    if constexpr (R == 2)                                                                                                                  \
    HL.set(data)

#define GET_REG_16()                                                                                                                       \
    static_assert(R < 4);                                                                                                                  \
    if constexpr (R == 0)                                                                                                                  \
        return BC.get();                                                                                                                   \
    if constexpr (R == 1)                                                                                                                  \
        return DE.get();                                                                                                                   \
    if constexpr (R == 2)                                                                                                                  \
    return HL.get()

namespace gbc
{
    CPU::CPU(Bus* bus) : m_bus(bus), m_registers(&bus->m_registers) {}

    void CPU::clock()
    {
        m_total_machine_cycles++;
        if (m_IME_scheduled)
        {
            m_IME_scheduled = false;
            m_IME = true;
        }

        if (m_remaining_machine_cycles == 0)
        {
            if (check_for_interrupt())
            {
                m_remaining_machine_cycles = 5;
            }
            else
            {
                auto operation = get_next_instruction();
                m_remaining_machine_cycles = (this->*operation)();
                ASSERT(m_remaining_machine_cycles != 0);
            }
        }
        m_remaining_machine_cycles--;
    }

    u32 CPU::step()
    {
        // Add remaining cycles for the current instruction, if any
        u32 clocks = m_remaining_machine_cycles;
        m_remaining_machine_cycles = 0;

        if (m_IME_scheduled)
        {
            m_IME_scheduled = false;
            m_IME = true;
        }

        if (check_for_interrupt())
        {
            clocks += 5;
        }
        else
        {
            auto operation = get_next_instruction();
            clocks += (this->*operation)();
        }

        m_total_machine_cycles += clocks;
        return clocks;
    }

    void CPU::run_until(u64 clock) {}

    bool CPU::check_for_interrupt()
    {
        u8 interrupt_enable = m_registers->m_interrupt_enable;
        u8 interrupt_flag = m_registers->m_interrupt_flag;

        if (m_IME)
        {
            for (u8 i = 0; i <= 4; i++)
            {
                u8 mask = (1 << i);
                if ((interrupt_enable & mask) && (interrupt_flag & mask))
                {
                    // Handle interrupt
                    m_IME = false;
                    m_registers->m_interrupt_flag &= ~mask;

                    push_stack(msb(PC));
                    push_stack(lsb(PC));
                    PC = 0x0040 + 8 * i;
                    return true;
                }
            }
        }
        return false;
    }

    Operation CPU::get_next_instruction()
    {
        u8 opcode = m_bus->cpu_read_byte(PC++);
        if (opcode == 0xCB)
        {
            opcode = m_bus->cpu_read_byte(PC++);
            return s_cb_opcodes[opcode];
        }

        return s_opcodes[opcode];
    }

    CPUData CPU::get_cpu_data() const
    {
        CPUData data;
        data.AF = AF.get();
        data.BC = BC.get();
        data.DE = DE.get();
        data.HL = HL.get();
        data.SP = SP;
        data.PC = PC;
        data.IME = m_IME;
        data.IME_scheduled = m_IME_scheduled;
        data.total_machine_cycles = m_total_machine_cycles;
        data.remaining_machine_cycles = m_remaining_machine_cycles;
        return data;
    }

    template <i32 R>
    inline u8 CPU::get_register()
    {
        static_assert(R < 8);

        if constexpr (R == 0)
            return BC.high;
        if constexpr (R == 1)
            return BC.low;
        if constexpr (R == 2)
            return DE.high;
        if constexpr (R == 3)
            return DE.low;
        if constexpr (R == 4)
            return HL.high;
        if constexpr (R == 5)
            return HL.low;
        if constexpr (R == 6)
            return m_bus->cpu_read_byte(HL.get());
        if constexpr (R == 7)
            return AF.acc;
    }

    template <>
    inline u8 CPU::get_register<8>()
    {
        return m_bus->cpu_read_byte(PC++);
    }

    template <i32 R>
    inline void CPU::write_register(u8 data)
    {
        static_assert(R < 8);

        if constexpr (R == 0)
            BC.high = data;
        if constexpr (R == 1)
            BC.low = data;
        if constexpr (R == 2)
            DE.high = data;
        if constexpr (R == 3)
            DE.low = data;
        if constexpr (R == 4)
            HL.high = data;
        if constexpr (R == 5)
            HL.low = data;
        if constexpr (R == 6)
            m_bus->cpu_write_byte(HL.get(), data);
        if constexpr (R == 7)
            AF.acc = data;
    }

    template <i32 R>
    inline u16 CPU::get_register_16()
    {
        GET_REG_16();
        if constexpr (R == 3)
            return SP;
    }

    template <i32 R>
    inline void CPU::write_register_16(u16 data)
    {
        SET_REG_16();
        if constexpr (R == 3)
            SP = data;
    }

    template <i32 R>
    inline u16 CPU::get_register2_16()
    {
        GET_REG_16();
        if constexpr (R == 3)
            return AF.get();
    }

    template <i32 R>
    inline void CPU::write_register2_16(u16 data)
    {
        SET_REG_16();
        if constexpr (R == 3)
            AF.set(data);
    }

    template <i32 F>
    inline u8 CPU::get_flag()
    {
        static_assert(F < 5);
        if constexpr (F == 0)
            return !AF.z;
        if constexpr (F == 1)
            return AF.z;
        if constexpr (F == 2)
            return !AF.c;
        if constexpr (F == 3)
            return AF.c;
        if constexpr (F == 4)
            return 1;
    }

    constexpr i32 ind_or_imm(i32 R, i32 ind_val, i32 other_val)
    {
        bool indirect_access = (R == 6 || R == 8);
        if (indirect_access)
            return ind_val;
        if (!indirect_access)
            return other_val;
    }

    inline u8 CPU::pop_stack() { return m_bus->cpu_read_byte(SP++); }
    inline void CPU::push_stack(u8 data) { m_bus->cpu_write_byte(--SP, data); }
    inline u16 CPU::read_16(u16& addr)
    {
        u16 lsb = m_bus->cpu_read_byte(addr++);
        u16 msb = m_bus->cpu_read_byte(addr++);
        return (msb << 8) | lsb;
    }
    inline void CPU::write_16(u16 addr, u16 data)
    {
        m_bus->cpu_write_byte(addr, data & 0x00FF);
        m_bus->cpu_write_byte(addr + 1, data >> 8);
    }

    // opcode: 0x00
    i32 CPU::nop() { return 1; }

    // opcode: 0b01yyyzzz
    template <i32 Ro, i32 Ri>
    i32 CPU::ld()
    {
        u8 data = get_register<Ri>();
        write_register<Ro>(data);

        constexpr bool indirect_access = (Ri == 6 || Ro == 6);
        if constexpr (indirect_access)
            return 2;

        if constexpr (!indirect_access)
            return 1;
    }

    // opcode: 0xF9
    i32 CPU::ld_sp_hl()
    {
        SP = HL.get();
        return 2;
    }

    // opcode: 0b00yyy110
    template <i32 R>
    i32 CPU::ld_imm()
    {
        u8 data = m_bus->cpu_read_byte(PC++);
        write_register<R>(data);
        return ind_or_imm(R, 3, 2);
    }

    // opcode: 0b00pp0001
    template <i32 R>
    i32 CPU::ld_imm_16()
    {
        u16 data = read_16(PC);
        write_register_16<R>(data);
        return 3;
    }

    // opcode: 0x02
    i32 CPU::ld_ind_bc_a()
    {
        m_bus->cpu_write_byte(BC.get(), AF.acc);
        return 2;
    }

    // opcode: 0x12
    i32 CPU::ld_ind_de_a()
    {
        m_bus->cpu_write_byte(DE.get(), AF.acc);
        return 2;
    }

    // opcode: 0x0A
    i32 CPU::ld_ind_a_bc()
    {
        u8 data = m_bus->cpu_read_byte(BC.get());
        AF.acc = data;
        return 2;
    }

    // opcode: 0x1A
    i32 CPU::ld_ind_a_de()
    {
        u8 data = m_bus->cpu_read_byte(DE.get());
        AF.acc = data;
        return 2;
    }

    // opcode: 0x22
    i32 CPU::ld_ind_hli_a()
    {
        u16 addr = HL.get();
        HL.set(addr + 1);
        m_bus->cpu_write_byte(addr, AF.acc);
        return 2;
    }

    // opcode: 0x32
    i32 CPU::ld_ind_hld_a()
    {
        u16 addr = HL.get();
        HL.set(addr - 1);
        m_bus->cpu_write_byte(addr, AF.acc);
        return 2;
    }

    // opcode: 0x2A
    i32 CPU::ld_ind_a_hli()
    {
        u16 addr = HL.get();
        HL.set(addr + 1);
        u8 data = m_bus->cpu_read_byte(addr);
        AF.acc = data;
        return 2;
    }

    // opcode: 0x3A
    i32 CPU::ld_ind_a_hld()
    {
        u16 addr = HL.get();
        HL.set(addr - 1);
        u8 data = m_bus->cpu_read_byte(addr);
        AF.acc = data;
        return 2;
    }

    // opcode: 0x08
    i32 CPU::ld_ind_nn_sp()
    {
        u16 addr = read_16(PC);
        write_16(addr, SP);
        return 5;
    }

    // opcode 0xFA
    i32 CPU::ld_ind_a_nn()
    {
        u16 addr = read_16(PC);
        AF.acc = m_bus->cpu_read_byte(addr);
        return 4;
    }

    // opcode: 0xEA
    i32 CPU::ld_ind_nn_a()
    {
        u16 addr = read_16(PC);
        m_bus->cpu_write_byte(addr, AF.acc);
        return 4;
    }

    // opcode: 0xE0
    i32 CPU::ldh_ind_n_a()
    {
        u16 addr = m_bus->cpu_read_byte(PC++);
        m_bus->cpu_write_byte(0xFF00 | addr, AF.acc);
        return 3;
    }

    // opcode: 0xF0
    i32 CPU::ldh_ind_a_n()
    {
        u16 addr = m_bus->cpu_read_byte(PC++);
        AF.acc = m_bus->cpu_read_byte(0xFF00 | addr);
        return 3;
    }

    // opcode: 0xF2
    i32 CPU::ldh_ind_a_c()
    {
        u16 addr = 0xFF00 | static_cast<u16>(BC.low);
        AF.acc = m_bus->cpu_read_byte(addr);
        return 2;
    }

    // opcode: 0xE2
    i32 CPU::ldh_ind_c_a()
    {
        u16 addr = 0xFF00 | static_cast<u16>(BC.low);
        m_bus->cpu_write_byte(addr, AF.acc);
        return 2;
    }

    // opcode: 0xF8
    i32 CPU::ld_hl_sp_n()
    {
        i8 offset = gbc::bit_cast<i8>(m_bus->cpu_read_byte(PC++));
        i32 result = SP + offset;

        // Flags
        AF.z = 0;
        AF.n = 0;
        AF.h = ((((SP & 0x000F) + (offset & 0x000F)) & 0x0010) == 0x0010);
        AF.c = ((((SP & 0x00FF) + (offset & 0x00FF)) & 0x0100) == 0x0100);

        HL.set(result & 0xFFFF);

        return 3;
    }

    // opcode: 0b10000zzz
    // opcode: 0xC6 (when R = 8)
    template <i32 R>
    i32 CPU::add()
    {
        u8 acc = AF.acc;
        u16 data = get_register<R>();
        u16 result = static_cast<u16>(AF.acc) + data;

        // Store result into acc
        AF.acc = result & 0x00FF;

        // Flags
        AF.z = (AF.acc == 0);
        AF.n = 0;
        AF.h = ((((data & 0x0F) + (acc & 0x0F)) & 0x10) == 0x10);
        AF.c = (result > 0xFF);

        return ind_or_imm(R, 2, 1);
    }

    // opcode: 0b00pp1001
    template <i32 R>
    i32 CPU::add_16()
    {
        u32 data = get_register_16<R>();
        u32 result = static_cast<u32>(HL.get()) + data;

        // Flags
        AF.n = 0;
        AF.h = ((((data & 0x0FFF) + (HL.get() & 0x0FFF)) & 0x1000) == 0x1000);
        AF.c = (result > 0xFFFF);

        // Store result into HL
        HL.set(result & 0xFFFF);

        return 2;
    }

    // opcode: 0xE8
    i32 CPU::add_sp_imm_8()
    {
        i8 offset = gbc::bit_cast<i8>(m_bus->cpu_read_byte(PC++));
        i32 result = SP + offset;

        // Flags
        AF.z = 0;
        AF.n = 0;
        AF.h = ((((SP & 0x000F) + (offset & 0x000F)) & 0x0010) == 0x0010);
        AF.c = ((((SP & 0x00FF) + (offset & 0x00FF)) & 0x0100) == 0x0100);

        SP = (result & 0xFFFF);
        return 4;
    }

    // opcode: 0b10001zzz
    // opcode: 0xCE (when R = 8)
    template <i32 R>
    i32 CPU::adc()
    {
        u8 acc = AF.acc;
        u16 data = get_register<R>();
        u16 result = static_cast<u16>(AF.acc) + data + AF.c;

        // Store result into acc
        AF.acc = result & 0x00FF;

        // Flags
        AF.z = (AF.acc == 0);
        AF.n = 0;
        AF.h = ((((data & 0x0F) + (acc & 0x0F) + AF.c) & 0x10) == 0x10);
        AF.c = (result > 0xFF);

        return ind_or_imm(R, 2, 1);
    }

    // opcode: 0b10010zzz
    // opcode: 0xD6 (when R = 8)
    template <i32 R>
    i32 CPU::sub()
    {
        u8 data = get_register<R>();
        u8 acc = AF.acc;

        // Store result into acc
        AF.acc = acc - data;

        // Flags
        AF.z = (data == acc);
        AF.n = 1;
        AF.h = ((acc & 0x0F) < (data & 0x0F));
        AF.c = (acc < data);

        return ind_or_imm(R, 2, 1);
    }

    // opcode: 0b10011zzz
    // opcode: 0xDE (when R = 8)
    template <i32 R>
    i32 CPU::sbc()
    {
        u16 data = get_register<R>();
        u16 result = AF.acc - data - AF.c;

        // Flags
        AF.z = ((result & 0xFF) == 0);
        AF.n = 1;
        AF.h = ((AF.acc ^ data ^ result) & 0x10) != 0;
        AF.c = ((result & ~0xFF) != 0);

        // Store result into acc
        AF.acc = result;

        return ind_or_imm(R, 2, 1);
    }

    // opcode: 0b10100zzz
    // opcode: 0xE6 (when R = 8)
    template <i32 R>
    i32 CPU::_and()
    {
        AF.acc &= get_register<R>();

        // Flags
        AF.z = (AF.acc == 0);
        AF.n = 0;
        AF.h = 1;
        AF.c = 0;

        return ind_or_imm(R, 2, 1);
    }

    // opcode: 0b10101zzz
    // opcode: 0xEE (when R = 8)
    template <i32 R>
    i32 CPU::_xor()
    {
        AF.acc ^= get_register<R>();

        // Flags
        AF.z = (AF.acc == 0);
        AF.n = 0;
        AF.h = 0;
        AF.c = 0;

        return ind_or_imm(R, 2, 1);
    }

    // opcode: 0b10110zzz
    // opcode: 0xF6 (when R = 8)
    template <i32 R>
    i32 CPU::_or()
    {
        AF.acc |= get_register<R>();

        // Flags
        AF.z = (AF.acc == 0);
        AF.n = 0;
        AF.h = 0;
        AF.c = 0;

        return ind_or_imm(R, 2, 1);
    }

    // opcode: 0b10111zzz
    // opcode: 0xFE (when R = 8)
    template <i32 R>
    i32 CPU::cp()
    {
        u8 data = get_register<R>();
        u8 acc = AF.acc;

        // Flags
        AF.z = (data == acc);
        AF.n = 1;
        AF.h = ((acc & 0x0F) < (data & 0x0F));
        AF.c = (acc < data);

        return ind_or_imm(R, 2, 1);
    }

    // opcode: 0b00yyy100
    template <i32 R>
    i32 CPU::inc()
    {
        u8 reg = get_register<R>();
        write_register<R>(reg + 1);

        // Flags
        AF.z = (reg == 0xFF);
        AF.n = 0;
        AF.h = (((reg & 0x0F) + 1) & 0x10) == 0x10;

        return ind_or_imm(R, 3, 1);
    }

    // opcode: 0b00yyy101
    template <i32 R>
    i32 CPU::dec()
    {
        u8 reg = get_register<R>();
        write_register<R>(reg - 1);

        // Flags
        AF.z = (reg == 1);
        AF.n = 1;
        AF.h = ((reg & 0x0F) == 0);

        return ind_or_imm(R, 3, 1);
    }

    // opcode: 0b00pp0011
    template <i32 R>
    i32 CPU::inc_16()
    {
        u16 reg = get_register_16<R>();
        write_register_16<R>(reg + 1);
        return 2;
    }

    // opcode: 0b00pp1011
    template <i32 R>
    i32 CPU::dec_16()
    {
        u16 reg = get_register_16<R>();
        write_register_16<R>(reg - 1);
        return 2;
    }

    // opcode: 0b00yyy000, where y=3..7
    template <i32 F>
    i32 CPU::jr()
    {
        i8 offset = gbc::bit_cast<i8>(m_bus->cpu_read_byte(PC++));
        if (get_flag<F>())
        {
            PC += offset;
            return 3;
        }
        return 2;
    }

    // opcode: 0x07
    i32 CPU::rlca()
    {
        AF.z = 0;
        AF.n = 0;
        AF.h = 0;
        AF.c = ((AF.acc & 0x80) != 0);
        AF.acc = (AF.acc << 1) | AF.c;
        return 1;
    }

    // opcode: 0x0F
    i32 CPU::rrca()
    {
        AF.z = 0;
        AF.n = 0;
        AF.h = 0;
        AF.c = AF.acc & 0x01;
        AF.acc = (AF.acc >> 1) | (AF.c << 7);
        return 1;
    }

    // opcode: 0x17
    i32 CPU::rla()
    {
        AF.z = 0;
        AF.n = 0;
        AF.h = 0;
        u8 c = AF.c;
        AF.c = ((AF.acc & 0x80) != 0);
        AF.acc = (AF.acc << 1) | c;
        return 1;
    }

    // opcode: 0x1F
    i32 CPU::rra()
    {
        AF.z = 0;
        AF.n = 0;
        AF.h = 0;
        u8 c = AF.c;
        AF.c = AF.acc & 0x01;
        AF.acc = (AF.acc >> 1) | (c << 7);
        return 1;
    }

    // opcode: 0x27
    i32 CPU::daa()
    {
        if (AF.n)
        {
            if (AF.c)
                AF.acc -= 0x60;
            if (AF.h)
                AF.acc -= 0x06;
        }
        else
        {
            if (AF.acc > 0x99 || AF.c)
            {
                AF.acc += 0x60;
                AF.c = 1;
            }
            if ((AF.acc & 0x0F) > 9 || AF.h)
                AF.acc += 0x06;
        }
        AF.z = (AF.acc == 0);
        AF.h = 0;

        return 1;
    }

    // opcode: 0x2F
    i32 CPU::cpl()
    {
        AF.acc = ~AF.acc;
        AF.n = 1;
        AF.h = 1;
        return 1;
    }

    // opcode: 0x37
    i32 CPU::scf()
    {
        AF.n = 0;
        AF.h = 0;
        AF.c = 1;
        return 1;
    }

    // opcode: 0x3F
    i32 CPU::ccf()
    {
        AF.n = 0;
        AF.h = 0;
        AF.c = 1 - AF.c;
        return 1;
    }

    // opcode: 0b11yyy111
    template <u8 ADDR>
    i32 CPU::rst()
    {
        push_stack(msb(PC));
        push_stack(lsb(PC));
        PC = static_cast<u16>(ADDR);
        return 4;
    }

    // opcode: 0b11pp0101
    template <i32 R>
    i32 CPU::push()
    {
        u16 reg = get_register2_16<R>();
        push_stack(msb(reg));
        push_stack(lsb(reg));
        return 4;
    }

    // opcode: 0b11pp0001
    template <i32 R>
    i32 CPU::pop()
    {
        u16 reg = pop_stack();
        reg |= (static_cast<u16>(pop_stack()) << 8);
        if constexpr (R == 3) // Clear lower flag bits when poping to AF
            reg &= 0xFFF0;
        write_register2_16<R>(reg);
        return 3;
    }

    // opcode: 0b110yy010 and 0xC3
    template <i32 F>
    i32 CPU::jp()
    {
        u16 addr = read_16(PC);
        if (get_flag<F>())
        {
            PC = addr;
            return 4;
        }
        return 3;
    }

    // opcode: 0xE9
    i32 CPU::jp_hl()
    {
        PC = HL.get();
        return 1;
    }

    // opcode: 0b110yy000 and 0xC9
    template <i32 F>
    i32 CPU::ret()
    {
        if (get_flag<F>())
        {
            PC = read_16(SP);
            if constexpr (F == 4)
                return 4;
            if constexpr (F != 4)
                return 5;
        }
        return 2;
    }

    // opcode: 0b110yy100 and 0xCD
    template <i32 F>
    i32 CPU::call()
    {
        u16 addr = read_16(PC);
        if (get_flag<F>())
        {
            push_stack(msb(PC));
            push_stack(lsb(PC));
            PC = addr;
            return 6;
        }
        return 3;
    }

    // opcode: 0xD9
    i32 CPU::reti()
    {
        PC = read_16(SP);
        m_IME = true;
        return 4;
    }

    // opcode: 0xF3
    i32 CPU::di()
    {
        m_IME = false;
        return 1;
    }

    // opcode: 0xFA
    i32 CPU::ei()
    {
        m_IME_scheduled = true;
        return 1;
    }

    // opcode: 0x10
    i32 CPU::stop()
    {
        // TODO
        LOG_WARN("Unimplemented opcode stop() called");
        return 1;
    }

    // opcode: 0x76
    i32 CPU::halt()
    {
        LOG_TRACE("halted");
        u8 IE = m_bus->m_registers.m_interrupt_enable;
        u8 IF = m_bus->m_registers.m_interrupt_flag;
        if (!(IE & IF & 0x1F)) // No pending interrupt
            m_halted = true;

        return 1;
    }

    // opcode: 0b00000zzz
    template <i32 R>
    i32 CPU::cb_rlc()
    {
        u8 reg = get_register<R>();
        AF.z = (reg == 0);
        AF.n = 0;
        AF.h = 0;
        AF.c = ((reg & 0x80) != 0);
        write_register<R>((reg << 1) | AF.c);
        return ind_or_imm(R, 4, 2);
    }

    // opcode: 0b00001zzz
    template <i32 R>
    i32 CPU::cb_rrc()
    {
        u8 reg = get_register<R>();
        AF.z = (reg == 0);
        AF.n = 0;
        AF.h = 0;
        AF.c = (reg & 0x01);
        write_register<R>((reg >> 1) | (AF.c << 7));
        return ind_or_imm(R, 4, 2);
    }

    // opcode: 0b00010zzz
    template <i32 R>
    i32 CPU::cb_rl()
    {
        u8 reg = get_register<R>();
        u8 c = AF.c;
        AF.c = ((reg & 0x80) != 0);
        reg = (reg << 1) | c;
        write_register<R>(reg);
        AF.z = (reg == 0);
        AF.n = 0;
        AF.h = 0;
        return ind_or_imm(R, 4, 2);
    }

    // opcode: 0b00011zzz
    template <i32 R>
    i32 CPU::cb_rr()
    {
        u8 reg = get_register<R>();
        u8 c = AF.c;
        AF.c = (reg & 0x01);
        reg = (reg >> 1) | (c << 7);
        write_register<R>(reg);
        AF.z = (reg == 0);
        AF.n = 0;
        AF.h = 0;
        return ind_or_imm(R, 4, 2);
    }

    // opcode: 0b00100zzz
    template <i32 R>
    i32 CPU::cb_sla()
    {
        u8 reg = get_register<R>();
        AF.c = ((reg & 0x80) != 0);
        reg <<= 1;
        write_register<R>(reg);
        AF.z = (reg == 0);
        AF.n = 0;
        AF.h = 0;
        return ind_or_imm(R, 4, 2);
    }

    // opcode: 0b00101zzz
    template <i32 R>
    i32 CPU::cb_sra()
    {
        u8 reg = get_register<R>();
        AF.c = (reg & 0x01);
        reg = (reg >> 1) | (reg & 0x80);
        write_register<R>(reg);
        AF.z = (reg == 0);
        AF.n = 0;
        AF.h = 0;
        return ind_or_imm(R, 4, 2);
    }

    // opcode: 0b00110zzz
    template <i32 R>
    i32 CPU::cb_swap()
    {
        u8 reg = get_register<R>();
        write_register<R>((reg << 4) | (reg >> 4));
        AF.z = (reg == 0);
        AF.n = 0;
        AF.h = 0;
        AF.c = 0;
        return ind_or_imm(R, 4, 2);
    }

    // opcode: 0b00111zzz
    template <i32 R>
    i32 CPU::cb_srl()
    {
        u8 reg = get_register<R>();
        AF.c = (reg & 0x01);
        reg = (reg >> 1);
        write_register<R>(reg);
        AF.z = (reg == 0);
        AF.n = 0;
        AF.h = 0;
        return ind_or_imm(R, 4, 2);
    }

    // opcode: 0b01yyyzzz
    template <i32 bit, i32 R>
    i32 CPU::cb_bit()
    {
        u8 reg = get_register<R>();
        AF.z = ((reg & (1 << bit)) == 0);
        AF.n = 0;
        AF.h = 1;
        return ind_or_imm(R, 3, 2);
    }

    // opcode: 0b10yyyzzz
    template <i32 bit, i32 R>
    i32 CPU::cb_res()
    {
        u8 reg = get_register<R>();
        write_register<R>(reg & ~(1 << bit));
        return ind_or_imm(R, 4, 2);
    }

    // opcode: 0b11yyyzzz
    template <i32 bit, i32 R>
    i32 CPU::cb_set()
    {
        u8 reg = get_register<R>();
        write_register<R>(reg | (1 << bit));
        return ind_or_imm(R, 4, 2);
    }

    i32 CPU::err()
    {
        LOG_ERROR("Invalid opcode");
        return 0;
    }

    // clang-format off
    Operation CPU::s_opcodes[256] =
    {
        &CPU::nop  , &CPU::ld_imm_16<0>, &CPU::ld_ind_bc_a , &CPU::inc_16<0>, &CPU::inc<0>, &CPU::dec<0>, &CPU::ld_imm<0>, &CPU::rlca, &CPU::ld_ind_nn_sp, &CPU::add_16<0>, &CPU::ld_ind_a_bc , &CPU::dec_16<0>, &CPU::inc<1>, &CPU::dec<1>, &CPU::ld_imm<1>, &CPU::rrca,
        &CPU::stop , &CPU::ld_imm_16<1>, &CPU::ld_ind_de_a , &CPU::inc_16<1>, &CPU::inc<2>, &CPU::dec<2>, &CPU::ld_imm<2>, &CPU::rla , &CPU::jr<4>       , &CPU::add_16<1>, &CPU::ld_ind_a_de , &CPU::dec_16<1>, &CPU::inc<3>, &CPU::dec<3>, &CPU::ld_imm<3>, &CPU::rra ,
        &CPU::jr<0>, &CPU::ld_imm_16<2>, &CPU::ld_ind_hli_a, &CPU::inc_16<2>, &CPU::inc<4>, &CPU::dec<4>, &CPU::ld_imm<4>, &CPU::daa , &CPU::jr<1>       , &CPU::add_16<2>, &CPU::ld_ind_a_hli, &CPU::dec_16<2>, &CPU::inc<5>, &CPU::dec<5>, &CPU::ld_imm<5>, &CPU::cpl ,
        &CPU::jr<2>, &CPU::ld_imm_16<3>, &CPU::ld_ind_hld_a, &CPU::inc_16<3>, &CPU::inc<6>, &CPU::dec<6>, &CPU::ld_imm<6>, &CPU::scf , &CPU::jr<3>       , &CPU::add_16<3>, &CPU::ld_ind_a_hld, &CPU::dec_16<3>, &CPU::inc<7>, &CPU::dec<7>, &CPU::ld_imm<7>, &CPU::ccf ,

        &CPU::ld<0, 0>, &CPU::ld<0, 1>, &CPU::ld<0, 2>, &CPU::ld<0, 3>, &CPU::ld<0, 4>, &CPU::ld<0, 5>, &CPU::ld<0, 6>, &CPU::ld<0, 7>, &CPU::ld<1, 0>, &CPU::ld<1, 1>, &CPU::ld<1, 2>, &CPU::ld<1, 3>, &CPU::ld<1, 4>, &CPU::ld<1, 5>, &CPU::ld<1, 6>, &CPU::ld<1, 7>,
        &CPU::ld<2, 0>, &CPU::ld<2, 1>, &CPU::ld<2, 2>, &CPU::ld<2, 3>, &CPU::ld<2, 4>, &CPU::ld<2, 5>, &CPU::ld<2, 6>, &CPU::ld<2, 7>, &CPU::ld<3, 0>, &CPU::ld<3, 1>, &CPU::ld<3, 2>, &CPU::ld<3, 3>, &CPU::ld<3, 4>, &CPU::ld<3, 5>, &CPU::ld<3, 6>, &CPU::ld<3, 7>,
        &CPU::ld<4, 0>, &CPU::ld<4, 1>, &CPU::ld<4, 2>, &CPU::ld<4, 3>, &CPU::ld<4, 4>, &CPU::ld<4, 5>, &CPU::ld<4, 6>, &CPU::ld<4, 7>, &CPU::ld<5, 0>, &CPU::ld<5, 1>, &CPU::ld<5, 2>, &CPU::ld<5, 3>, &CPU::ld<5, 4>, &CPU::ld<5, 5>, &CPU::ld<5, 6>, &CPU::ld<5, 7>,
        &CPU::ld<6, 0>, &CPU::ld<6, 1>, &CPU::ld<6, 2>, &CPU::ld<6, 3>, &CPU::ld<6, 4>, &CPU::ld<6, 5>, &CPU::halt    , &CPU::ld<6, 7>, &CPU::ld<7, 0>, &CPU::ld<7, 1>, &CPU::ld<7, 2>, &CPU::ld<7, 3>, &CPU::ld<7, 4>, &CPU::ld<7, 5>, &CPU::ld<7, 6>, &CPU::ld<7, 7>,

        &CPU::add<0> , &CPU::add<1> , &CPU::add<2> , &CPU::add<3> , &CPU::add<4> , &CPU::add<5> , &CPU::add<6> , &CPU::add<7> , &CPU::adc<0> , &CPU::adc<1> , &CPU::adc<2> , &CPU::adc<3> , &CPU::adc<4> , &CPU::adc<5> , &CPU::adc<6> , &CPU::adc<7> ,
        &CPU::sub<0> , &CPU::sub<1> , &CPU::sub<2> , &CPU::sub<3> , &CPU::sub<4> , &CPU::sub<5> , &CPU::sub<6> , &CPU::sub<7> , &CPU::sbc<0> , &CPU::sbc<1> , &CPU::sbc<2> , &CPU::sbc<3> , &CPU::sbc<4> , &CPU::sbc<5> , &CPU::sbc<6> , &CPU::sbc<7> ,
        &CPU::_and<0>, &CPU::_and<1>, &CPU::_and<2>, &CPU::_and<3>, &CPU::_and<4>, &CPU::_and<5>, &CPU::_and<6>, &CPU::_and<7>, &CPU::_xor<0>, &CPU::_xor<1>, &CPU::_xor<2>, &CPU::_xor<3>, &CPU::_xor<4>, &CPU::_xor<5>, &CPU::_xor<6>, &CPU::_xor<7>,
        &CPU::_or<0> , &CPU::_or<1> , &CPU::_or<2> , &CPU::_or<3> , &CPU::_or<4> , &CPU::_or<5> , &CPU::_or<6> , &CPU::_or<7> , &CPU::cp<0>  , &CPU::cp<1>  , &CPU::cp<2>  , &CPU::cp<3>  , &CPU::cp<4>  , &CPU::cp<5>  , &CPU::cp<6>  , &CPU::cp<7>  ,

        &CPU::ret<0>     , &CPU::pop<0>, &CPU::jp<0>      , &CPU::jp<4>, &CPU::call<0>, &CPU::push<0>, &CPU::add<8> , &CPU::rst<0x00>, &CPU::ret<1>      , &CPU::ret<4>  , &CPU::jp<1>      , &CPU::err, &CPU::call<1>, &CPU::call<4>, &CPU::adc<8> , &CPU::rst<0x08>,
        &CPU::ret<2>     , &CPU::pop<1>, &CPU::jp<2>      , &CPU::err  , &CPU::call<2>, &CPU::push<1>, &CPU::sub<8> , &CPU::rst<0x10>, &CPU::ret<3>      , &CPU::reti    , &CPU::jp<3>      , &CPU::err, &CPU::call<3>, &CPU::err    , &CPU::sbc<8> , &CPU::rst<0x18>,
        &CPU::ldh_ind_n_a, &CPU::pop<2>, &CPU::ldh_ind_c_a, &CPU::err  , &CPU::err    , &CPU::push<2>, &CPU::_and<8>, &CPU::rst<0x20>, &CPU::add_sp_imm_8, &CPU::jp_hl   , &CPU::ld_ind_nn_a, &CPU::err, &CPU::err    , &CPU::err    , &CPU::_xor<8>, &CPU::rst<0x28>,
        &CPU::ldh_ind_a_n, &CPU::pop<3>, &CPU::ldh_ind_a_c, &CPU::di   , &CPU::err    , &CPU::push<3>, &CPU::_or<8> , &CPU::rst<0x30>, &CPU::ld_hl_sp_n  , &CPU::ld_sp_hl, &CPU::ld_ind_a_nn, &CPU::ei , &CPU::err    , &CPU::err    , &CPU::cp<8>  , &CPU::rst<0x38>,
    };

    Operation CPU::s_cb_opcodes[256] =
    {
        &CPU::cb_rlc<0> , &CPU::cb_rlc<1> , &CPU::cb_rlc<2> , &CPU::cb_rlc<3> , &CPU::cb_rlc<4> , &CPU::cb_rlc<5> , &CPU::cb_rlc<6> , &CPU::cb_rlc<7> , &CPU::cb_rrc<0>, &CPU::cb_rrc<1>, &CPU::cb_rrc<2>, &CPU::cb_rrc<3>, &CPU::cb_rrc<4>, &CPU::cb_rrc<5>, &CPU::cb_rrc<6>, &CPU::cb_rrc<7>,
        &CPU::cb_rl<0>  , &CPU::cb_rl<1>  , &CPU::cb_rl<2>  , &CPU::cb_rl<3>  , &CPU::cb_rl<4>  , &CPU::cb_rl<5>  , &CPU::cb_rl<6>  , &CPU::cb_rl<7>  , &CPU::cb_rr<0> , &CPU::cb_rr<1> , &CPU::cb_rr<2> , &CPU::cb_rr<3> , &CPU::cb_rr<4> , &CPU::cb_rr<5> , &CPU::cb_rr<6> , &CPU::cb_rr<7> ,
        &CPU::cb_sla<0> , &CPU::cb_sla<1> , &CPU::cb_sla<2> , &CPU::cb_sla<3> , &CPU::cb_sla<4> , &CPU::cb_sla<5> , &CPU::cb_sla<6> , &CPU::cb_sla<7> , &CPU::cb_sra<0>, &CPU::cb_sra<1>, &CPU::cb_sra<2>, &CPU::cb_sra<3>, &CPU::cb_sra<4>, &CPU::cb_sra<5>, &CPU::cb_sra<6>, &CPU::cb_sra<7>,
        &CPU::cb_swap<0>, &CPU::cb_swap<1>, &CPU::cb_swap<2>, &CPU::cb_swap<3>, &CPU::cb_swap<4>, &CPU::cb_swap<5>, &CPU::cb_swap<6>, &CPU::cb_swap<7>, &CPU::cb_srl<0>, &CPU::cb_srl<1>, &CPU::cb_srl<2>, &CPU::cb_srl<3>, &CPU::cb_srl<4>, &CPU::cb_srl<5>, &CPU::cb_srl<6>, &CPU::cb_srl<7>,

        &CPU::cb_bit<0, 0>, &CPU::cb_bit<0, 1>, &CPU::cb_bit<0, 2>, &CPU::cb_bit<0, 3>, &CPU::cb_bit<0, 4>, &CPU::cb_bit<0, 5>, &CPU::cb_bit<0, 6>, &CPU::cb_bit<0, 7>, &CPU::cb_bit<1, 0>, &CPU::cb_bit<1, 1>, &CPU::cb_bit<1, 2>, &CPU::cb_bit<1, 3>, &CPU::cb_bit<1, 4>, &CPU::cb_bit<1, 5>, &CPU::cb_bit<1, 6>, &CPU::cb_bit<1, 7>,
        &CPU::cb_bit<2, 0>, &CPU::cb_bit<2, 1>, &CPU::cb_bit<2, 2>, &CPU::cb_bit<2, 3>, &CPU::cb_bit<2, 4>, &CPU::cb_bit<2, 5>, &CPU::cb_bit<2, 6>, &CPU::cb_bit<2, 7>, &CPU::cb_bit<3, 0>, &CPU::cb_bit<3, 1>, &CPU::cb_bit<3, 2>, &CPU::cb_bit<3, 3>, &CPU::cb_bit<3, 4>, &CPU::cb_bit<3, 5>, &CPU::cb_bit<3, 6>, &CPU::cb_bit<3, 7>,
        &CPU::cb_bit<4, 0>, &CPU::cb_bit<4, 1>, &CPU::cb_bit<4, 2>, &CPU::cb_bit<4, 3>, &CPU::cb_bit<4, 4>, &CPU::cb_bit<4, 5>, &CPU::cb_bit<4, 6>, &CPU::cb_bit<4, 7>, &CPU::cb_bit<5, 0>, &CPU::cb_bit<5, 1>, &CPU::cb_bit<5, 2>, &CPU::cb_bit<5, 3>, &CPU::cb_bit<5, 4>, &CPU::cb_bit<5, 5>, &CPU::cb_bit<5, 6>, &CPU::cb_bit<5, 7>,
        &CPU::cb_bit<6, 0>, &CPU::cb_bit<6, 1>, &CPU::cb_bit<6, 2>, &CPU::cb_bit<6, 3>, &CPU::cb_bit<6, 4>, &CPU::cb_bit<6, 5>, &CPU::cb_bit<6, 6>, &CPU::cb_bit<6, 7>, &CPU::cb_bit<7, 0>, &CPU::cb_bit<7, 1>, &CPU::cb_bit<7, 2>, &CPU::cb_bit<7, 3>, &CPU::cb_bit<7, 4>, &CPU::cb_bit<7, 5>, &CPU::cb_bit<7, 6>, &CPU::cb_bit<7, 7>,

        &CPU::cb_res<0, 0>, &CPU::cb_res<0, 1>, &CPU::cb_res<0, 2>, &CPU::cb_res<0, 3>, &CPU::cb_res<0, 4>, &CPU::cb_res<0, 5>, &CPU::cb_res<0, 6>, &CPU::cb_res<0, 7>, &CPU::cb_res<1, 0>, &CPU::cb_res<1, 1>, &CPU::cb_res<1, 2>, &CPU::cb_res<1, 3>, &CPU::cb_res<1, 4>, &CPU::cb_res<1, 5>, &CPU::cb_res<1, 6>, &CPU::cb_res<1, 7>,
        &CPU::cb_res<2, 0>, &CPU::cb_res<2, 1>, &CPU::cb_res<2, 2>, &CPU::cb_res<2, 3>, &CPU::cb_res<2, 4>, &CPU::cb_res<2, 5>, &CPU::cb_res<2, 6>, &CPU::cb_res<2, 7>, &CPU::cb_res<3, 0>, &CPU::cb_res<3, 1>, &CPU::cb_res<3, 2>, &CPU::cb_res<3, 3>, &CPU::cb_res<3, 4>, &CPU::cb_res<3, 5>, &CPU::cb_res<3, 6>, &CPU::cb_res<3, 7>,
        &CPU::cb_res<4, 0>, &CPU::cb_res<4, 1>, &CPU::cb_res<4, 2>, &CPU::cb_res<4, 3>, &CPU::cb_res<4, 4>, &CPU::cb_res<4, 5>, &CPU::cb_res<4, 6>, &CPU::cb_res<4, 7>, &CPU::cb_res<5, 0>, &CPU::cb_res<5, 1>, &CPU::cb_res<5, 2>, &CPU::cb_res<5, 3>, &CPU::cb_res<5, 4>, &CPU::cb_res<5, 5>, &CPU::cb_res<5, 6>, &CPU::cb_res<5, 7>,
        &CPU::cb_res<6, 0>, &CPU::cb_res<6, 1>, &CPU::cb_res<6, 2>, &CPU::cb_res<6, 3>, &CPU::cb_res<6, 4>, &CPU::cb_res<6, 5>, &CPU::cb_res<6, 6>, &CPU::cb_res<6, 7>, &CPU::cb_res<7, 0>, &CPU::cb_res<7, 1>, &CPU::cb_res<7, 2>, &CPU::cb_res<7, 3>, &CPU::cb_res<7, 4>, &CPU::cb_res<7, 5>, &CPU::cb_res<7, 6>, &CPU::cb_res<7, 7>,

        &CPU::cb_set<0, 0>, &CPU::cb_set<0, 1>, &CPU::cb_set<0, 2>, &CPU::cb_set<0, 3>, &CPU::cb_set<0, 4>, &CPU::cb_set<0, 5>, &CPU::cb_set<0, 6>, &CPU::cb_set<0, 7>, &CPU::cb_set<1, 0>, &CPU::cb_set<1, 1>, &CPU::cb_set<1, 2>, &CPU::cb_set<1, 3>, &CPU::cb_set<1, 4>, &CPU::cb_set<1, 5>, &CPU::cb_set<1, 6>, &CPU::cb_set<1, 7>,
        &CPU::cb_set<2, 0>, &CPU::cb_set<2, 1>, &CPU::cb_set<2, 2>, &CPU::cb_set<2, 3>, &CPU::cb_set<2, 4>, &CPU::cb_set<2, 5>, &CPU::cb_set<2, 6>, &CPU::cb_set<2, 7>, &CPU::cb_set<3, 0>, &CPU::cb_set<3, 1>, &CPU::cb_set<3, 2>, &CPU::cb_set<3, 3>, &CPU::cb_set<3, 4>, &CPU::cb_set<3, 5>, &CPU::cb_set<3, 6>, &CPU::cb_set<3, 7>,
        &CPU::cb_set<4, 0>, &CPU::cb_set<4, 1>, &CPU::cb_set<4, 2>, &CPU::cb_set<4, 3>, &CPU::cb_set<4, 4>, &CPU::cb_set<4, 5>, &CPU::cb_set<4, 6>, &CPU::cb_set<4, 7>, &CPU::cb_set<5, 0>, &CPU::cb_set<5, 1>, &CPU::cb_set<5, 2>, &CPU::cb_set<5, 3>, &CPU::cb_set<5, 4>, &CPU::cb_set<5, 5>, &CPU::cb_set<5, 6>, &CPU::cb_set<5, 7>,
        &CPU::cb_set<6, 0>, &CPU::cb_set<6, 1>, &CPU::cb_set<6, 2>, &CPU::cb_set<6, 3>, &CPU::cb_set<6, 4>, &CPU::cb_set<6, 5>, &CPU::cb_set<6, 6>, &CPU::cb_set<6, 7>, &CPU::cb_set<7, 0>, &CPU::cb_set<7, 1>, &CPU::cb_set<7, 2>, &CPU::cb_set<7, 3>, &CPU::cb_set<7, 4>, &CPU::cb_set<7, 5>, &CPU::cb_set<7, 6>, &CPU::cb_set<7, 7>,
    };
    // clang-format on
}