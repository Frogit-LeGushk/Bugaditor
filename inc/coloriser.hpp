#pragma once

#include <utility>
#include <ncurses.h>

struct Colorizer
{
    Colorizer(std::pair<int,int> default_colors)
    {
        is_support_     = has_colors();
        default_colors_ = default_colors;
        start_color();
        init_colorpairs__();
    }

    enum
    {
        BLACK = 0,
        BLUE,
        GREEN,
        CYAN,
        RED,
        MAGENT,
        YELLOW,
        WHITE,
        UNKNOWN
    };

    void setColor(int fg, int bg)
    {
        if (isSupport())
            attron(COLOR_PAIR(colornum__(fg, bg)));
    }

    void setDefault(void)
    {
        const auto [fg, bg] = default_colors_;
        setColor(fg, bg);
    }

    void setReverse(void) const
    {
        attron(A_REVERSE);
    }

    void offReverse(void) const
    {
        attroff(A_REVERSE);
    }

    bool isSupport(void) const
    {
        return is_support_;
    }

private:
    void init_colorpairs__(void)
    {
        int fg, bg;
        int colorpair;

        for (bg = 0; bg <= 7; bg++) {
            for (fg = 0; fg <= 7; fg++) {
                colorpair = colornum__(fg, bg);
                init_pair(colorpair, curs_color__(fg), curs_color__(bg));
            }
        }
    }

    int colornum__(int fg, int bg)
    {
        int B, bbb, ffff;

        B   = (1 << 7);
        bbb = (7 & bg) << 4;
        ffff= (7 & fg);

        return (B | bbb | ffff);
    }

    short curs_color__(int fg)
    {
        switch (7 & fg)
        {
        case 0: return (COLOR_BLACK);
        case 1: return (COLOR_BLUE);
        case 2: return (COLOR_GREEN);
        case 3: return (COLOR_CYAN);
        case 4: return (COLOR_RED);
        case 5: return (COLOR_MAGENTA);
        case 6: return (COLOR_YELLOW);
        case 7: return (COLOR_WHITE);
        default: return(COLOR_BLACK);
        }
    }

private:
    bool is_support_;
    std::pair<int,int> default_colors_;
};
