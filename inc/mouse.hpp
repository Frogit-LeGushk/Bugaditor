#pragma once

#include <utility>
#include <cstddef>
#include "text.hpp"

struct Mouse
{
    Mouse(Text& text);
    ~Mouse();

    void setPos(int x, int y);
    std::pair<int,int> getPos(void) const;

    void setIsClicked(bool isClicked);
    bool isClicked(void) const;

    void setIsFocused(bool isFocused);
    bool isFocused(void) const;

    template<class Callback>
    void pressEvent(Callback&& callback)
    {
        setIsClicked(true);
        setIsFocused(true);

        saveFirstFixedPos__(text_, callback);
        text_.setCursorPos(getFixedPos().first);
    }

    template<class Callback>
    void releaseEvent(Callback&& callback)
    {
        setIsClicked(false);
        saveSecondFixedPos__(text_, callback);

        const auto [first, second] = getFixedPos();
        text_.setCursorPos(second);

        if (first == second)
            setIsFocused(false);
    }

    template<class Callback>
    void moveEvent(Callback&& callback, int x, int y)
    {
        setPos(x, y);

        if (isClicked() && isFocused())
        {
            saveSecondFixedPos__(text_, callback);
            text_.setCursorPos(getFixedPos().second);
        }
    }

    std::pair<size_t,size_t> getFixedPos(void) const;

private:
    template<class Callback>
    void saveFirstFixedPos__(Text const& text, Callback&& callback)
    {
        first_  = callback(text, mousePos_);
    }

    template<class Callback>
    void saveSecondFixedPos__(Text const& text, Callback&& callback)
    {
        second_ = callback(text, mousePos_);
    }

private:
    Text& text_;
    std::pair<int,int> mousePos_ = {0, 0};
    size_t first_   = 0;
    size_t second_  = 0;
    bool IS_CLICKED_= false;
    bool IS_FOCUSED_= false;
};
