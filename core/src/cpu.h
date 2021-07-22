#pragma once
#include "bus.h"
#include "util/common.h"

namespace gbc
{
    // Returns number of m-cycles needed to complete the instruction
    class CPU;
    using Operation = i32 (CPU::*)();

    struct Register
    {
        u8 low = 0;
        u8 high = 0;
        inline u16 get() { return (static_cast<u16>(high) << 8) | static_cast<u16>(low); }
        inline void set(u16 value)
        {
            low = value & 0x00FF;
            high = (value & 0xFF00) >> 8;
        }
    };

    struct CPUData
    {
        u16 AF, BC, DE, HL, SP, PC;
        bool IME;
        bool IME_scheduled;
        u64 total_machine_cycles;
        u32 remaining_machine_cycles;
    };

    class CPU
    {
    public:
        CPU(Bus* bus);
        void clock();
        void step();
        void run_until(u64 clock);
        CPUData get_cpu_data();

    private:
        struct
        {
            u8 unused : 4;
            u8 z : 1; // zero flag
            u8 n : 1; // subtraction flag
            u8 h : 1; // half carry flag
            u8 c : 1; // carry flag
            u8 acc;   // accumulator
            inline void set(u16 data) { std::memcpy(this, &data, sizeof(u16)); }
            inline u16 get() { return std::bit_cast<u16>(*this); }
        } AF;
        Register BC, DE, HL;
        u16 SP, PC = 0;
        bool IME = false;
        bool IME_scheduled = false;
        u64 m_total_machine_cycles = 0;
        u32 m_remaining_machine_cycles = 0;

        Bus* m_bus;
        static Operation s_opcodes[256];
        static Operation s_cb_opcodes[256];

    private:
        Operation get_next_instruction();

    private:
        // Helpers
        template <i32 R>
        inline u8 get_register();
        template <i32 R>
        inline void write_register(u8 data);
        template <i32 R>
        inline u16 get_register_16();
        template <i32 R>
        inline void write_register_16(u16 data);
        template <i32 R>
        inline u16 get_register2_16();
        template <i32 R>
        inline void write_register2_16(u16 data);
        template <i32 F>
        inline u8 get_flag();
        inline u8 pop_stack();
        inline void push_stack(u8 data);
        inline u16 read_16(u16& addr);
        inline void write_16(u16 addr, u16 data);

        // Opcodes
        i32 nop();
        template <i32 Ri, i32 Ro>
        i32 ld();
        i32 ld_sp_hl();
        template <i32 R>
        i32 ld_imm();
        template <i32 R>
        i32 ld_imm_16();
        i32 ld_ind_bc_a();
        i32 ld_ind_de_a();
        i32 ld_ind_a_bc();
        i32 ld_ind_a_de();
        i32 ld_ind_hli_a();
        i32 ld_ind_hld_a();
        i32 ld_ind_a_hli();
        i32 ld_ind_a_hld();
        i32 ld_ind_nn_sp();
        i32 ld_ind_a_nn();
        i32 ld_ind_nn_a();
        i32 ldh_ind_n_a();
        i32 ldh_ind_a_n();
        i32 ldh_ind_a_c();
        i32 ldh_ind_c_a();
        i32 ld_hl_sp_n();
        template <i32 R>
        i32 add();
        template <i32 R>
        i32 add_16();
        i32 add_sp_imm_8();
        template <i32 R>
        i32 adc();
        template <i32 R>
        i32 sub();
        template <i32 R>
        i32 sbc();
        // c++ alternative tokens reserves keywords 'and', 'xor' and 'or'. Hence the use of underscores
        template <i32 R>
        i32 _and();
        template <i32 R>
        i32 _xor();
        template <i32 R>
        i32 _or();
        template <i32 R>
        i32 cp();
        template <i32 R>
        i32 inc();
        template <i32 R>
        i32 dec();
        template <i32 R>
        i32 inc_16();
        template <i32 R>
        i32 dec_16();
        template <i32 F>
        i32 jr();
        i32 rlca();
        i32 rrca();
        i32 rla();
        i32 rra();
        i32 daa();
        i32 cpl();
        i32 scf();
        i32 ccf();
        template <u8 ADDR>
        i32 rst();
        template <i32 R>
        i32 push();
        template <i32 R>
        i32 pop();
        template <i32 F>
        i32 jp();
        i32 jp_hl();
        template <i32 F>
        i32 ret();
        template <i32 F>
        i32 call();
        i32 reti();
        i32 di();
        i32 ei();
        i32 stop();
        i32 halt();

        // cb prefixed opcodes
        template <i32 R>
        i32 cb_rlc();
        template <i32 R>
        i32 cb_rrc();
        template <i32 R>
        i32 cb_rl();
        template <i32 R>
        i32 cb_rr();
        template <i32 R>
        i32 cb_sla();
        template <i32 R>
        i32 cb_sra();
        template <i32 R>
        i32 cb_swap();
        template <i32 R>
        i32 cb_srl();
        template <i32 bit, i32 R>
        i32 cb_bit();
        template <i32 bit, i32 R>
        i32 cb_res();
        template <i32 bit, i32 R>
        i32 cb_set();

        i32 err(); // All invalid opcodes captured here
    };
}