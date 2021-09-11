#include "debug/cpu.h"
#include <imgui.h>

namespace app
{
    void CPUWindow::draw_window(const char* title, const GBC& gbc, MessageQueue& message_queue)
    {
        static const char* flags[8] = {".", ".", ".", ".", "c", "h", "n", "z"};
        static const ImVec4 green = {0, 1, 0, 1};
        static const ImVec4 red = {1, 0, 0, 1};

        ImGui::Begin(title);

        CPUData d = gbc.get_cpu_data();
        ImGui::Text("IME: %i", d.IME);
        ImGui::Text("AF: $%04X", d.AF);
        ImGui::SameLine();
        for (i32 i = 7; i >= 0; i--)
        {
            bool active = ((lsb(d.AF) & (1 << i)) > 0);
            ImGui::TextColored(active ? green : red, flags[i]);
            if (i != 0)
                ImGui::SameLine(0, 0);
        }
        ImGui::Text("BC: $%04X", d.BC);
        ImGui::Text("DE: $%04X", d.DE);
        ImGui::Text("HL: $%04X", d.HL);
        ImGui::Text("SP: $%04X", d.SP);
        ImGui::Text("PC: $%04X", d.PC);
        ImGui::Text("Total Machine Cycles: %u", d.total_machine_cycles);

        if (ImGui::Button("step"))
        {
            message_queue.push_back(Message(MessageType::STEP_EMULATION));
        }

        ImGui::SameLine();
        if (ImGui::Checkbox("Pause", &m_paused))
        {
            message_queue.push_back(Message(MessageType::TOGGLE_PAUSE));
        }

        // static char addr_input_buf[64];
        // if (ImGui::InputText("goto", addr_input_buf, 64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        //{
        //    sscanf(addr_input_buf, "%X", &m_wait_addr);
        //    u16 addr = static_cast<u16>(m_wait_addr);
        //    while (gbc.get_pc() != addr)
        //    {
        //        gbc.clock();
        //    }
        //}

        if (ImGui::Button("reset"))
        {
            m_paused = true;
            message_queue.push_back(Message(MessageType::RESET_GBC));
            // m_wait_addr = 0;
        }

        //#ifdef PLATFORM_WINDOWS
        //        if (ImGui::Button("open..."))
        //        {
        //            char filepath[256];
        //            OPENFILENAMEA ofn;
        //
        //            ZeroMemory(&ofn, sizeof(ofn));
        //            ofn.lStructSize = sizeof(ofn);
        //            ofn.lpstrFile = filepath;
        //            // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
        //            // use the contents of szFile to initialize itself.
        //            ofn.lpstrFile[0] = '\0';
        //            ofn.nMaxFile = 256;
        //            ofn.lpstrFilter = "All\0*.*\0GB (.gb)\0*.gb\0";
        //            ofn.nFilterIndex = 2;
        //            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        //
        //            if (GetOpenFileNameA(&ofn))
        //            {
        //                m_paused = true;
        //                m_step_count = 0;
        //                m_wait_addr = 0;
        //                gbc = create_scope<GBC>(filepath);
        //            }
        //        }
        //#endif

        if (ImGui::Button("dump bg tilemap"))
        {
            auto tilemap = gbc.dump_bg_tile_map();
            write_file("tilemap.bin", tilemap);
        }
        ImGui::End();
    }
}