#include "debug/disassembly.h"

#include <imgui.h>

namespace app
{
    const ImVec4 before_pc_col = {0.5, 0.5, 0.5, 1};
    const ImVec4 pc_col = {0.0745, 0.9294, 0.2156, 1};
    const ImVec4 after_pc_col = {1, 1, 1, 1};

    DisassemblyWindow::DisassemblyWindow() : m_base_addrs(m_num_lines), m_lines(m_num_lines) {}

    void DisassemblyWindow::reset()
    {
        std::for_each(m_lines.begin(), m_lines.end(), [](Line& line) { line[0] = '\0'; });
    }

    void DisassemblyWindow::fill_lines(const GBC& gbc, u16 pc)
    {

        auto it = m_base_addrs.begin();
        for (i32 i = 0; it != m_base_addrs.end(); it++, i++)
        {
            if (*it == pc && !is_empty(m_lines[i]))
                break;
        }
        if (it == m_base_addrs.end())
        {
            for (i32 i = 0; i < m_offset; i++)
            {
                m_lines[i][0] = '\0';
            }

            for (i32 i = m_offset; i < m_num_lines; i++)
            {
                m_base_addrs[i] = pc;
                pc += disassemble(m_lines[i], gbc, pc);
            }
        }
        else
        {
            // shift our lines
            i32 shift = (it - m_base_addrs.begin()) - m_offset;
            if (shift < 0) // shift right
            {
                shift = -shift;
                for (i32 i = m_num_lines - 1; i >= shift; i--)
                {
                    m_base_addrs[i] = m_base_addrs[i - shift];
                    m_lines[i] = m_lines[i - shift];
                }
                // clear lines
                for (i32 i = 0; i < shift; i++)
                {
                    m_lines[i][0] = '\0';
                }
            }
            else if (shift > 0) // shift left
            {
                for (i32 i = 0; i < m_num_lines - shift; i++)
                {
                    m_base_addrs[i] = m_base_addrs[i + shift];
                    m_lines[i] = m_lines[i + shift];
                }
                // fill lines
                u16 pc = m_base_addrs[m_num_lines - shift - 1];
                // start one line before we need to start filling lines
                for (i32 i = m_num_lines - shift - 1; i < m_num_lines; i++)
                {
                    m_base_addrs[i] = pc;
                    pc += disassemble(m_lines[i], gbc, pc);
                }
            }
        }
    }

    void DisassemblyWindow::draw_window(const char* title, const GBC& gbc, u16 pc)
    {
        fill_lines(gbc, pc);
        ImGui::Begin(title);

        for (i32 i = 0; i < m_num_lines; i++)
        {
            if (i < m_offset)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, before_pc_col);
            }
            else if (i == m_offset)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, pc_col);
            }
            else if (i > m_offset)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, after_pc_col);
            }
            ImGui::Text(m_lines[i].data());
            ImGui::PopStyleColor();
        }

        ImGui::End();
    }

    // Decoding opcodes based off this document:
    // https://gb-archive.github.io/salvage/decoding_gbz80_opcodes/Decoding%20Gamboy%20Z80%20Opcodes.html
    static inline u8 get_x(u8 opcode) { return opcode >> 6; }
    static inline u8 get_y(u8 opcode) { return (opcode & 0x3F) >> 3; }
    static inline u8 get_z(u8 opcode) { return opcode & 0x07; }
    static inline u8 get_p(u8 opcode) { return (opcode & 0x3F) >> 4; }
    static inline u8 get_q(u8 opcode) { return (opcode & 0x08) != 0; }

    static const char* r[8] = {"B", "C", "D", "E", "H", "L", "(HL)", "A"};
    static const char* rp[4] = {"BC", "DE", "HL", "SP"};
    static const char* rp2[4] = {"BC", "DE", "HL", "AF"};
    static const char* cc[4] = {"NZ", "Z", "NC", "C"};
    static const char* alu[8] = {"add A,", "adc A,", "sub", "sbc A,", "and", "xor", "or", "cp"};
    static const char* rot[8] = {"rlc", "rrc", "rl", "rr", "sla", "sra", "swap", "srl"};
    static const char* bit[3] = {"bit", "res", "set"};
    static const char* ldy[8] = {"(BC), A", "A, (BC)", "(DE), A", "A, (DE)", "(HL+), A", "A, (HL+)", "(HL-), A", "A, (HL-)"};
    static const char* id[2] = {"inc", "dec"};

    static inline u16 get_u16(const GBC& gbc, u16& pc)
    {
        u16 addr = gbc.peek_byte(pc++);
        addr |= (static_cast<u16>(gbc.peek_byte(pc++)) << 8);
        return addr;
    }
    static inline char sign(u8 value) { return (gbc::bit_cast<i8>(value) < 0) ? '-' : '+'; }
    static inline u8 mag(u8 value)
    {
        i8 signed_val = gbc::bit_cast<i8>(value);
        return (signed_val < 0) ? -signed_val : signed_val;
    }

    u16 DisassemblyWindow::disassemble(Line& line, const GBC& gbc, u16 pc)
    {
        u16 base = pc;
        u8 opcode = gbc.peek_byte(pc++);
        if (opcode == 0xCB)
        {
            opcode = gbc.peek_byte(pc++);
            u8 x = get_x(opcode);
            u8 y = get_y(opcode);
            u8 z = get_z(opcode);
            if (x == 0)
                std::sprintf(line.data(), "$%.4X: CB %.2X    %s %s", base, opcode, rot[y], r[z]);
            else
                std::sprintf(line.data(), "$%.4X: CB %.2X    %s %hhu, %s", base, opcode, bit[x - 1], y, r[z]);
            return 2;
        }
        else
        {
            u8 x = get_x(opcode);
            u8 y = get_y(opcode);
            u8 z = get_z(opcode);
            u8 p = get_p(opcode);
            u8 q = get_q(opcode);

            static std::set<u8> invalid_opcodes = {0xD3, 0xDB, 0xDD, 0xE3, 0xE4, 0xEB, 0xEC, 0xED, 0xF4, 0xFC, 0xFD};
            if (invalid_opcodes.find(opcode) != invalid_opcodes.end()) // Handle invalid opcode
            {
                // Print up to 3 bytes
                u8 next1 = gbc.peek_byte(pc++);
                u8 next2 = gbc.peek_byte(pc++);
                if (invalid_opcodes.find(next1) == invalid_opcodes.end())
                {
                    std::sprintf(line.data(), "$%.4X: %.2X", base, opcode);
                    return 1;
                }
                else if (invalid_opcodes.find(next2) == invalid_opcodes.end())
                {
                    std::sprintf(line.data(), "$%.4X: %.2X %.2X", base, opcode, next1);
                    return 2;
                }
                else
                {
                    std::sprintf(line.data(), "$%.4X: %.2X %.2X %.2X", base, opcode, next1, next2);
                    return 3;
                }
            }

            switch (opcode) // Stand alone opcodes
            {
            case 0x00: std::sprintf(line.data(), "$%.4X: 00       nop", base); return 1;
            case 0x07: std::sprintf(line.data(), "$%.4X: 07       rlca", base); return 1;
            case 0x08:
            {
                u16 addr = get_u16(gbc, pc);
                std::sprintf(line.data(), "$%.4X: 08 %.2X %.2X ld (%.4X), SP", base, lsb(addr), msb(addr), addr);
                return 3;
            }
            case 0x0F: std::sprintf(line.data(), "$%.4X: 0F       rrca", base); return 1;
            case 0x10: std::sprintf(line.data(), "$%.4X: 10       stop", base); return 1;
            case 0x17: std::sprintf(line.data(), "$%.4X: 17       rla", base); return 1;
            case 0x18:
            { // Perhaps we should format the jump offset as a signed int?
                u8 jmp = gbc.peek_byte(pc++);
                std::sprintf(line.data(), "$%.4X: 18 %.2X    jr %c%.2X", base, jmp, sign(jmp), mag(jmp));
                return 2;
            }
            case 0x1F: std::sprintf(line.data(), "$%.4X: 1F       rra", base); return 1;
            case 0x27: std::sprintf(line.data(), "$%.4X: 27       daa", base); return 1;
            case 0x2F: std::sprintf(line.data(), "$%.4X: 2F       cpl", base); return 1;
            case 0x37: std::sprintf(line.data(), "$%.4X: 37       scf", base); return 1;
            case 0x3F: std::sprintf(line.data(), "$%.4X: 3F       ccf", base); return 1;
            case 0x76: std::sprintf(line.data(), "$%.4X: 76       halt", base); return 1;
            case 0xC3:
            {
                u16 addr = get_u16(gbc, pc);
                std::sprintf(line.data(), "$%.4X: C3 %.2X %.2X jp %.4X", base, lsb(addr), msb(addr), addr);
                return 3;
            }
            case 0xC9: std::sprintf(line.data(), "$%.4X: C9       ret", base); return 1;
            case 0xCD:
            {
                u16 addr = get_u16(gbc, pc);
                std::sprintf(line.data(), "$%.4X: CD %.2X %.2X call %.4X", base, lsb(addr), msb(addr), addr);
                return 3;
            }
            case 0xD9: std::sprintf(line.data(), "$%.4X: D9       reti", base); return 1;
            case 0xE0:
            {
                u8 addr = gbc.peek_byte(pc++);
                std::sprintf(line.data(), "$%.4X: E0 %.2X    lhd (%.2X), A", base, addr, addr);
                return 2;
            }
            case 0xE2: std::sprintf(line.data(), "$%.4X: E2       ld (C), A", base); return 1;
            case 0xE8:
            { // Perhaps we should format the add as a signed int?
                u8 add = gbc.peek_byte(pc++);
                std::sprintf(line.data(), "$%.4X: E8 %.2X    add SP, %c%.2X", base, add, sign(add), mag(add));
                return 2;
            }
            case 0xE9: std::sprintf(line.data(), "$%.4X: E9       jp HL", base); return 1;
            case 0xEA:
            {
                u16 addr = get_u16(gbc, pc);
                std::sprintf(line.data(), "$%.4X: EA %.2X %.2X ld (%.4X), A", base, lsb(addr), msb(addr), addr);
                return 3;
            }
            case 0xF0:
            {
                u8 addr = gbc.peek_byte(pc++);
                std::sprintf(line.data(), "$%.4X: F0 %.2X    lhd A, (%.2X)", base, addr, addr);
                return 2;
            }
            case 0xF2: std::sprintf(line.data(), "$%.4X: F2       ld A, (C)", base); return 1;
            case 0xF3: std::sprintf(line.data(), "$%.4X: F3       di", base); return 1;
            case 0xF8:
            { // Perhaps we should format the offset as a signed int?
                u8 add = gbc.peek_byte(pc++);
                std::sprintf(line.data(), "$%.4X: F8 %.2X    lh HL, SP %c %.2X", base, add, sign(add), mag(add));
                return 2;
            }
            case 0xF9: std::sprintf(line.data(), "$%.4X: F9       LD SP, HL", base); return 1;
            case 0xFA:
            {
                u16 addr = get_u16(gbc, pc);
                std::sprintf(line.data(), "$%.4X: FA %.2X %.2X ld A, (%.4X)", base, lsb(addr), msb(addr), addr);
                return 3;
            }
            case 0xFB: std::sprintf(line.data(), "$%.4X: FB       ei", base); return 1;
            }

            if (x == 0)
            {
                if (z == 0)
                { // Perhaps we should format the jump as a signed int?
                    ASSERT(y >= 4);
                    u8 jmp = gbc.peek_byte(pc++);
                    std::sprintf(line.data(), "$%.4X: %.2X %.2X    jr %s, %c%.2X", base, opcode, jmp, cc[y - 4], sign(jmp), mag(jmp));
                    return 2;
                }
                if (z == 1)
                {
                    if (q == 0)
                    {
                        u16 addr = get_u16(gbc, pc);
                        std::sprintf(line.data(), "$%.4X: %.2X %.2X %.2X ld %s, %.4X", base, opcode, lsb(addr), msb(addr), rp[p], addr);
                        return 3;
                    }
                    else
                    {
                        std::sprintf(line.data(), "$%.4X: %.2X       add HL, %s", base, opcode, rp[p]);
                        return 1;
                    }
                }
                if (z == 2)
                {
                    std::sprintf(line.data(), "$%.4X: %.2X       ld %s", base, opcode, ldy[y]);
                    return 1;
                }
                if (z == 3)
                {
                    std::sprintf(line.data(), "$%.4X: %.2X       %s %s", base, opcode, id[q], rp[p]);
                    return 1;
                }
                if (z == 4 || z == 5)
                {
                    std::sprintf(line.data(), "$%.4X: %.2X       %s %s", base, opcode, id[z & 1], r[y]);
                    return 1;
                }
                if (z == 6)
                {
                    u8 imm = gbc.peek_byte(pc++);
                    std::sprintf(line.data(), "$%.4X: %.2X %.2X    ld %s, %.2X", base, opcode, imm, r[y], imm);
                    return 2;
                }
            }

            // ld opcodes
            if (x == 1)
            {
                std::sprintf(line.data(), "$%.4X: %.2X       ld %s, %s", base, opcode, r[y], r[z]);
                return 1;
            }

            // alu opcodes
            if (x == 2)
            {
                std::sprintf(line.data(), "$%.4X: %.2X       %s %s", base, opcode, alu[y], r[z]);
                return 1;
            }

            if (x == 3)
            {
                if (z == 0)
                {
                    ASSERT(y < 4);
                    std::sprintf(line.data(), "$%.4X: %.2X       ret %s", base, opcode, cc[y]);
                    return 1;
                }
                if (z == 1)
                {
                    ASSERT(q == 0);
                    std::sprintf(line.data(), "$%.4X: %.2X       pop %s", base, opcode, rp2[p]);
                    return 1;
                }
                if (z == 2)
                {
                    ASSERT(y < 4);
                    u16 addr = get_u16(gbc, pc);
                    std::sprintf(line.data(), "$%.4X: %.2X %.2X %.2X jp %s, %.4X", base, opcode, lsb(addr), msb(addr), cc[y], addr);
                    return 3;
                }
                if (z == 4)
                {
                    ASSERT(y < 4);
                    u16 addr = get_u16(gbc, pc);
                    std::sprintf(line.data(), "$%.4X: %.2X %.2X %.2X call %s, %.4X", base, opcode, lsb(addr), msb(addr), cc[y], addr);
                    return 3;
                }
                if (z == 5)
                {
                    ASSERT(q == 0);
                    std::sprintf(line.data(), "$%.4X: %.2X       push %s", base, opcode, rp2[p]);
                    return 1;
                }
                if (z == 6)
                {
                    u8 imm = gbc.peek_byte(pc++);
                    std::sprintf(line.data(), "$%.4X: %.2X %.2X    %s %.2X", base, opcode, imm, alu[y], imm);
                    return 2;
                }
                if (z == 7)
                {
                    std::sprintf(line.data(), "$%.4X: %.2X       rst %.2X", base, opcode, y * 8);
                    return 1;
                }
            }
        }
        ASSERT(false); // Unreachable code
        return 0;
    }
}