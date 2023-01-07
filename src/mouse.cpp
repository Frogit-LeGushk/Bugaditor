#include "../inc/mouse.hpp"

#include <string>
#include <iostream>

Mouse::Mouse(Text& text) : text_(text)
{
    const std::string ENABLE_REPORT_MOUSE_POSITION = "\033[?1003h\n";
    std::cout << ENABLE_REPORT_MOUSE_POSITION << std::endl;
}

Mouse::~Mouse()
{
    const std::string DISABLE_REPORT_MOUSE_POSITION = "\033[?1003l\n";
    std::cout << DISABLE_REPORT_MOUSE_POSITION << std::endl;
}

void Mouse::setPos(int x, int y)
{
    mousePos_ = std::make_pair(x, y);
}

std::pair<int,int>  Mouse::getPos(void) const
{
    return mousePos_;
}

void Mouse::setIsClicked(bool isClicked)
{
    IS_CLICKED_ = isClicked;
}

bool Mouse::isClicked(void) const
{
    return IS_CLICKED_;
}

void Mouse::setIsFocused(bool isFocused)
{
    IS_FOCUSED_ = isFocused;
}

bool Mouse::isFocused(void) const
{
    return IS_FOCUSED_;
}

std::pair<size_t,size_t> Mouse::getFixedPos(void) const
{
    return std::make_pair(first_, second_);
}


