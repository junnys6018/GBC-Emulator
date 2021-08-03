#pragma once
#include "bus.h"
#include "cpu.h"
#include "divider.h"
#include "io_registers.h"
#include "ppu.h"
#include "util/log.h"

namespace gbc
{
    class GBC
    {
    public:
        GBC(const std::string& file);
        void step();

        // Getters
        inline CPUData get_cpu_data() const { return m_cpu.get_cpu_data(); }
        inline u16 get_pc() const { return m_cpu.get_pc(); }
        inline u16 get_sp() const { return m_cpu.get_sp(); }
        inline u8 peek_byte(u16 addr) const { return m_bus.peek_byte(addr); }
        inline const u8* get_vram() const { return m_bus.get_vram(); }
        inline const u8* get_wram() const { return m_bus.get_wram(); }
        inline const u8* get_hram() const { return m_bus.get_hram(); }
        inline const u8* get_rom() const { return m_cartridge->get_rom(); }
        inline const IORegisters& get_io_reg() const { return m_bus.m_registers; }
        inline const u32* get_framebuffer() const { return m_ppu.get_framebuffer(); }
        u32 next_timer_event() const;

#if defined(GBC_COMPILE_TESTS)
        friend class ::GBCTests;
#endif
        friend class PPU;

    private:
        Scope<Cartridge> m_cartridge;
        Bus m_bus;
        CPU m_cpu;
        PPU m_ppu;
        const Divider m_div;
        u64 m_total_t_cycles = 0;
        GBCMode m_mode = COMPATIBILITY_MODE;

        void clock_timers(u32 t_clocks);
    };


}
