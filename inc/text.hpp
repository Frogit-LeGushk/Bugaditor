#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cassert>
#include <cstring>

class Text
{
    void parseText__(void);

public:
    Text();
    Text(std::string const& text);
    Text(std::string && text);

    size_t  getOffset(void) const;
    void    setOffset(const size_t offset);
    void    incOffset(void);
    void    decOffset(void);

    size_t  getCntLines(void) const;
    size_t  getLineSize(const size_t line) const;
    std::shared_ptr<std::string>
            getLine(const size_t line) const;

    size_t  getPos(const size_t line, const size_t left) const;
    std::pair<size_t, size_t>
            getPos(const size_t pos) const;
    size_t  getCursorPos(void) const;
    void    setCursorPos(const size_t pos);

    enum class EMoveCursor : uint8_t
    {
        UP = 1,
        DOWN,
        LEFT,
        RIGHT
    };

    void    moveCursor(EMoveCursor move);

    std::shared_ptr<std::string>
            copyRange(size_t first, size_t second) const;
    void    delRange(size_t first, size_t second);

    void    insAfterCursor(std::string const& substring);
    void    delBeforeCursor(const size_t size = 1);

    size_t  getCursorLine(void) const;
    void    setCursorLine(const size_t line, const bool isStartLine = true);
    size_t  getCursorLeft(void) const;
    void    setCursorLeft(const size_t left);
    void    setCursorStartLine(void);
    void    setCursorEndLine(void);
    void    setCursorStartFile(void);
    void    setCursorEndFile(void);

    std::string const&
            getText(void) const;

    struct Token
    {
        size_t offset;
        std::string token;
        size_t hash;
    };

    std::vector<Token> const&
            getTokens(void) const;
private:
    std::string text_;
    std::vector<size_t> lines_;
    std::pair<size_t, size_t> cursor_{0,0};
    std::vector<Token> tokens_;
    size_t offset_ = 0;
};
