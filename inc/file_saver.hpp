#pragma once

#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstddef>

struct FileSaver
{
    FileSaver(Text& text, std::string const& filename)
        : text_(text), filename_(filename)
    {
        if (!load())
        {
            std::cerr << "Could't load file: " << filename_ << std::endl;
            exit(1);
        }
    }

    bool save() const
    {
        std::ofstream output(filename_, std::ofstream::trunc);
        int retry               = 1;
        const size_t max_retry  = 3;

        while (!output && retry != max_retry)
        {
            sleep(1);
            output.open(filename_, std::ofstream::trunc);
            retry++;
        }

        if (!output)
            return false;

        const char * text = text_.getText().c_str();
        const size_t size = text_.getText().size();
        output.seekp(0, std::ios_base::beg);
        output.write(text, size);
        output.close();

        return true;
    }

    bool load(void)
    {
        std::ifstream input(filename_);
        int retry               = 1;
        const size_t max_retry  = 3;

        while (!input && retry != max_retry)
        {
            sleep(1);
            input.open(filename_);
            retry++;
        }

        if (!input)
            return false;

        std::stringstream sstream;
        sstream << input.rdbuf();
        input.close();

        const size_t first = 0;
        const size_t second= text_.getText().size() - 1;

        text_.delRange(first, second);
        text_.insAfterCursor(std::move(sstream.str()));
        text_.setCursorPos(first);
        return true;
    }

private:
    Text& text_;
    std::string filename_;
};
