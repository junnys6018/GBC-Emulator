#pragma once
#include "common.h"
#include <functional>

namespace app
{

    template <typename... Args>
    class EventEmitter;

    template <typename... Args>
    class ScopedEventListener
    {
    private:
        EventEmitter<Args...>* m_emitter;
        u32 m_id;

    public:
        ScopedEventListener(EventEmitter<Args...>* emitter, u32 listener_id) : m_emitter(emitter), m_id(listener_id) {}
        ScopedEventListener(const ScopedEventListener<Args...>& other) = delete;
        ScopedEventListener<Args...>& operator=(const ScopedEventListener<Args...>& other) = delete;
        ScopedEventListener(ScopedEventListener<Args...>&& other) : m_emitter(other.m_emitter), m_id(other.m_id) { other.m_emitter = nullptr; }
        ScopedEventListener<Args...>& operator=(ScopedEventListener<Args...>&& other)
        {
            std::swap(m_emitter, other.m_emitter);
            std::swap(m_id, other.m_id);
            return *this;
        }
        ~ScopedEventListener()
        {
            if (m_emitter)
                m_emitter->remove_event_listener(m_id);
        }
    };

    template <typename... Args>
    class EventEmitter
    {
    public:
        using Listener = std::function<bool(const Args&...)>;

    private:
        struct ListenerContainer
        {
            Listener callback;
            u32 id;
        };

        u32 m_next_id = 0;
        std::vector<ListenerContainer> m_listeners;

    public:
        EventEmitter() = default;

        uint32_t add_event_listener(const Listener& listener)
        {
            u32 id = get_next_id();
            m_listeners.push_back({listener, id});
            return id;
        }

        ScopedEventListener<Args...> add_scoped_event_listener(const Listener& listener)
        {
            return ScopedEventListener<Args...>(this, add_event_listener(listener));
        }

        bool remove_event_listener(u32 listener)
        {
            auto it = std::find(m_listeners.begin(), m_listeners.end(),
                                [&](const ListenerContainer& container) { return container.id == listener; });
            if (it != m_listeners.end())
            {
                m_listeners.erase(it);
                return true;
            }
            return false;
        }

        bool trigger(const Args&... args) const
        {
            bool handled = false;
            for (size_t i = 0; i < m_listeners.size() && !handled; i++)
            {
                handled |= m_listeners[i].callback(args...);
            }
            return handled;
        }

    private:
        u32 get_next_id() { return m_next_id++; }
    };
}
