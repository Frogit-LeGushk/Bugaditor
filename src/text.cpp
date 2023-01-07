#include "../inc/text.hpp"

void Text::parseText__(void)
{
    if (getText().size() > 0 && getText()[getText().size() - 1] != '\n')
        text_.push_back('\n');
    if (getText().size() == 0)
        text_.push_back('\n');

    lines_.resize(0);
    tokens_.resize(0);

    for (size_t i = 0; i < getText().size(); i++)
        if (getText()[i] == '\n')
            lines_.push_back(i);

    const std::string copyText(getText());
    const char * start = copyText.c_str();
    const char * delim = "<>.,:;-\n []{}()=+&|^\"'!$%*/@";
    const char * token = strtok(const_cast<char *>(start), delim);

    while (token != NULL)
    {
        const size_t offset     = (size_t)token - (size_t)start;
        const size_t hash       = std::hash<std::string>{}(token);

        if (token[0] != '\0')
        {
            tokens_.push_back({offset, token, hash});
            token = strtok(NULL, delim);
        }
    }
}

Text::Text() : text_("\n")
    { parseText__(); }
Text::Text(std::string const& text) : text_(text)
    { parseText__(); }
Text::Text(std::string && text) : text_(std::move(text))
    { parseText__(); }

size_t Text::getOffset(void) const
{
    return offset_;
}

void Text::setOffset(const size_t offset)
{
    if (lines_.size() > 0 && offset >= lines_.size())
        offset_ = lines_.size() - 1;
    else if (lines_.size() == 0 && offset > lines_.size())
        offset_ = 0;
    else
        offset_ = offset;
}

void Text::incOffset(void)
{
    setOffset(getOffset() + 1);
}

void Text::decOffset(void)
{
    setOffset(getOffset() - 1);
}

size_t Text::getCntLines(void) const
{
    return lines_.size();
}

size_t Text::getLineSize(const size_t line) const
{
    assert(line < getCntLines());

    if (line == 0)
        return lines_[0];
    else
        return lines_[line] - lines_[line - 1] - 1;
}

std::shared_ptr<std::string> Text::getLine(const size_t line) const
{
    assert(line < getCntLines());
    const size_t lineSize = getLineSize(line);

    const auto end    = getText().cbegin() + lines_[line];
    const auto begin  = end - lineSize;

    return std::make_shared<std::string>(begin, end);
}

size_t Text::getPos(const size_t line, const size_t left) const
{
    assert(line < getCntLines());
    assert(left <= getLineSize(line));

    const size_t startLine = (line > 0) ? (lines_[line - 1] + 1) : 0;
    return startLine + left;
}

std::pair<size_t, size_t> Text::getPos(const size_t pos) const
{
    assert(pos < getText().size());

    size_t line = 0;
    while (pos > lines_[line]) line++;

    const size_t lineSize   = getLineSize(line);
    const size_t left       = pos - (lines_[line] - lineSize);
    return std::make_pair(line, left);
}

size_t Text::getCursorPos(void) const
{
    return getPos(getCursorLine(), getCursorLeft());
}

void Text::setCursorPos(const size_t pos)
{
    assert(pos < getText().size());

    size_t line = 0;
    while (pos > lines_[line]) line++;

    setCursorLine(line);
    const size_t lineSize   = getLineSize(line);
    const size_t left       = pos - (lines_[line] - lineSize);
    setCursorLeft(left);
}

void Text::moveCursor(Text::EMoveCursor move)
{
    auto [line, left] = cursor_;

    switch (move)
    {
    case Text::EMoveCursor::UP:
        if (line > 0)
        {
            line--;
            if (left > getLineSize(line))
                left = getLineSize(line);
        }
        break;
    case Text::EMoveCursor::DOWN:
        if (0 < getCntLines() && line < getCntLines() - 1)
        {
            line++;
            if (left > getLineSize(line))
                left = getLineSize(line);
        }
        break;
    case Text::EMoveCursor::LEFT:
        if (left > 0)
            left--;
        else if (line > 0)
        {
            line--;
            left = getLineSize(line);
        }
        break;
    case Text::EMoveCursor::RIGHT:
        if (left < getLineSize(line))
            left++;
        else if (0 < getCntLines() && line < getCntLines() - 1)
        {
            line++;
            left = 0;
        }
        break;
    }

    cursor_ = std::make_pair(line, left);
}

std::shared_ptr<std::string> Text::copyRange(size_t first, size_t second) const
{
    if (first > second)
        std::swap(first, second);
    assert(second < getText().size());

    const auto begin    = getText().cbegin() + first;
    const auto end      = getText().cbegin() + second + 1;
    return std::make_shared<std::string>(begin, end);
}

void Text::delRange(size_t first, size_t second)
{
    if (first > second)
        std::swap(first, second);
    assert(second < getText().size());

    const size_t size = second - first + 1;
    size_t pos        = getCursorPos();

    text_.erase(first, size);
    parseText__();
    assert(getText().size() > 0);

    if (getText().size() <= pos)
        setCursorPos(getText().size() - 1);
    else if (second <= pos)
        setCursorPos(pos - size + 1);
    else if (first < pos)
        setCursorPos(first);
}

void Text::insAfterCursor(std::string const& substring)
{
    const size_t pos = getCursorPos();
    text_.insert(pos, substring);
    parseText__();
    setCursorPos(pos + substring.size());
}

void Text::delBeforeCursor(const size_t size)
{
    if (getCursorLeft() > 0 || getCursorLine() > 0)
    {
        const size_t oldPos = getCursorPos();
        const size_t delSize= std::min(size, oldPos);
        const size_t newPos = oldPos - delSize;

        setCursorPos(newPos);
        text_.erase(newPos, delSize);
        parseText__();
    }
}

size_t Text::getCursorLine(void) const
{
    return cursor_.first;
}

void Text::setCursorLine(const size_t line, const bool isStartLine)
{
    assert(line < getCntLines());
    cursor_.first = line;

    if (isStartLine)
        setCursorStartLine();
    else
        setCursorEndLine();
}

size_t Text::getCursorLeft(void) const
{
    return cursor_.second;
}

void Text::setCursorLeft(const size_t left)
{
    const size_t line = cursor_.first;
    assert(left <= getLineSize(line));
    cursor_.second = left;
}

void Text::setCursorStartLine(void)
{
    const size_t line = cursor_.first;
    cursor_ = std::make_pair(line, 0);
}

void Text::setCursorEndLine(void)
{
    const size_t line = cursor_.first;
    cursor_ = std::make_pair(line, getLineSize(line));
}

void Text::setCursorStartFile(void)
{
    setCursorLine(0);
    setCursorStartLine();
}

void Text::setCursorEndFile(void)
{
    setCursorLine(getCntLines()-1);
    setCursorEndLine();
}

std::string const& Text::getText(void) const
{
    return text_;
}

std::vector<Text::Token> const& Text::getTokens(void) const
{
    return tokens_;
}


