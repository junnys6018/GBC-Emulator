#pragma once
#include "common.h"
#include <functional>

namespace app
{

    template <typename T>
    class EventEmitter;

    template <typename T>
    class ScopedEventListener
    {
    private:
        EventEmitter<T>* m_emitter;
        u32 m_id;

    public:
        ScopedEventListener(EventEmitter<T>* emitter, u32 listener_id);
        ScopedEventListener(const ScopedEventListener<T>& other) = delete;
        ScopedEventListener<T>& operator=(const ScopedEventListener<T>& other) = delete;
        ScopedEventListener(ScopedEventListener<T>&& other);
        ScopedEventListener<T>& operator=(ScopedEventListener<T>&& other);
        ~ScopedEventListener();
    };

    template <typename T>
    class EventEmitter
    {
    public:
        using Listener = std::function<bool(const T&)>;

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

        ScopedEventListener<T> add_scoped_event_listener(const Listener& listener)
        {
            return ScopedEventListener<T>(this, add_event_listener(listener));
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

        bool trigger(const T& evt) const
        {
            bool handled = false;
            for (size_t i = 0; i < m_listeners.size() && !handled; i++)
            {
                handled |= m_listeners[i].callback(evt);
            }
            return handled;
        }

    private:
        u32 get_next_id() { return m_next_id++; }
    };

    template <typename T>
    ScopedEventListener<T>::ScopedEventListener(EventEmitter<T>* emitter, u32 listener_id) : m_emitter(emitter), m_id(listener_id)
    {
    }

    template <typename T>
    ScopedEventListener<T>::ScopedEventListener(ScopedEventListener<T>&& other) : m_emitter(other.m_emitter), m_id(other.m_id)
    {
        other.m_emitter = nullptr;
    }

    template <typename T>
    ScopedEventListener<T>& ScopedEventListener<T>::operator=(ScopedEventListener<T>&& other)
    {
        std::swap(m_emitter, other.m_emitter);
        std::swap(m_id, other.m_id);
        return *this;
    }

    template <typename T>
    ScopedEventListener<T>::~ScopedEventListener()
    {
        if (m_emitter)
            m_emitter->remove_event_listener(m_id);
    }

}
