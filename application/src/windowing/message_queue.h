#pragma once
#include <mutex>
#include <queue>
#include "common.h"

namespace app
{
    enum class MessageType
    {
        NULL_MESSAGE,

        // sent from gbc thread
        GBC_WINDOW_SHUTDOWN,

        // sent from debug thread
        STEP_EMULATION,
        TOGGLE_PAUSE,
        RESET_GBC,
    };

    struct Message
    {
    public:
        Message(MessageType type = MessageType::NULL_MESSAGE) : m_type(type) {}
        Message(const Message&) = default;
        Message& operator=(const Message&) = default;
    public:
        MessageType m_type;
    };

    class MessageQueue
    {
    public:
        void push_back(const Message& message);
        bool pop_front(Message& message);
    private:
        std::queue<Message> m_messages;
        std::mutex m_mutex;
    };
}