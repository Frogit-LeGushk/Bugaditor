#pragma once

#include <functional>
#include <unordered_map>

struct Keyboard
{
    Keyboard(const std::function<void(void)> default_handler)
        : default_handler_(default_handler) {}

    void addEventHandler(const int event, const std::function<void(void)> handler)
    {
        handlers_[event] = handler;
    }

    void deleteEventHandler(const int event)
    {
        handlers_.erase(event);
    }

    void callEventHandler(const int event)
    {
        if (handlers_.find(event) != handlers_.end())
            handlers_[event]();
        else
            default_handler_();
    }

private:
    std::unordered_map<int, std::function<void(void)>> handlers_;
    std::function<void(void)> default_handler_;
};
