#pragma once
#include "bus.h"
#include "cpu.h"
#include "timer.h"
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
        void clock();
        void set_keys(Keys keys);

        // Getters
        inline CPUData get_cpu_data() const { return m_cpu.get_cpu_data(); }
        inline u16 get_pc() const { return m_cpu.get_pc(); }
        inline u16 get_sp() const { return m_cpu.get_sp(); }
        inline u8 peek_byte(u16 addr) const { return m_bus.peek_byte(addr); }
        inline const u8* get_vram() const { return m_bus.get_vram(); }
        inline const u8* get_wram() const { return m_bus.get_wram(); }
        inline const u8* get_hram() const { return m_bus.get_hram(); }
        inline const u8* get_oam() const { return m_bus.get_oam(); }
        inline const u8* get_rom() const { return m_cartridge->get_rom(); }
        inline const IORegisters& get_io_reg() const { return m_bus.m_registers; }
        inline const u32* get_framebuffer() const { return m_ppu.get_framebuffer(); }
        inline std::vector<u8> dump_bg_tile_map() const { return m_ppu.dump_bg_tile_map(); }

#if defined(GBC_COMPILE_TESTS)
        friend class ::GBCTests;
#endif
        friend class PPU;
        friend class IORegisters;
        friend class Bus;
        friend class Timer;

        Timer m_timer;
    private:
        Scope<Cartridge> m_cartridge;
        Bus m_bus;
        CPU m_cpu;
        PPU m_ppu;

        u64 m_total_t_cycles = 0;
        GBCMode m_mode = GBCMode::COMPATIBILITY;
        Keys m_keys;
        u32 m_oam_dma_transfer = 0;

        void do_dma_cycles(u32 count);
    };

}
