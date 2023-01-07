#pragma once

#include <string>

struct Buffer
{
    void clear()
    {
        buffer_.resize(0);
    }

    bool isEmpty(void) const
    {
        return buffer_.size() == 0;
    }

    std::string const& get(void) const
    {
        return buffer_;
    }

    void set(std::string const& buffer, size_t pos1, size_t pos2)
    {
        if (pos1 > pos2)
            std::swap(pos1, pos2);

        assert(pos2 < buffer.size());

        if (pos1 != pos2)
            buffer_ = std::move(buffer.substr(pos1, pos2 - pos1 + 1));
    }

    void set(std::string && buffer)
    {
        buffer_ = std::move(buffer);
    }

private:
    std::string buffer_;
};
