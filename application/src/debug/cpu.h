#pragma once
#include "common.h"
#include "windowing/message_queue.h"
#include <gbc.h>

namespace app
{
    using namespace gbc;
    class CPUWindow
    {
    public:
        CPUWindow() = default;
        void draw_window(const char* title, const GBC& gbc, MessageQueue& message_queue);

    private:
        bool m_paused = true;
    };
}