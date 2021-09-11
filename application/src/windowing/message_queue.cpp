#include "message_queue.h"

namespace app
{
    void MessageQueue::push_back(const Message& message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_messages.push(message);
    }

    bool MessageQueue::pop_front(Message& message)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_messages.empty())
            return false;

        message = m_messages.front();
        m_messages.pop();
        return true;
    }

};