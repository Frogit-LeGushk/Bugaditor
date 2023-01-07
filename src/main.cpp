#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <functional>
#include <tuple>
#include <unordered_map>

#include <ncurses.h>

#include "../inc/text.hpp"
#include "../inc/mouse.hpp"
#include "../inc/buffer.hpp"
#include "../inc/coloriser.hpp"
#include "../inc/file_saver.hpp"
#include "../inc/keyboard.hpp"

using namespace std;

enum CASTOM_KEY_EVENTS {
    KEY_ENT             = 10,
    KEY_CTRL_V          = 22,
    KEY_CTRL_C          = 3,
    KEY_CTRL_S          = 19,
    KEY_CTRL_B          = 2,
    KEY_CTRL_E          = 5,
    KEY_CTRL_ARR_LEFT   = 545,
    KEY_CTRL_ARR_RIGHT  = 560,
    KEY_ESC             = 27
};


using tokens_t = unordered_map<size_t, string>;

size_t          screenToText (Text const& text, const pair<int,int> mousePos);
pair<int,int>   calculateText(Text const& text, Mouse const& mouse, Colorizer& colorizer, tokens_t const& tokens);
tokens_t        readTokens(string const& filename);


int main(int argc, char ** crgv)
{
    if (argc != 3)
    {
        cerr << "Err: enter input file" << endl;
        return 1;
    }

    int         key;
    const char * filename   = crgv[1];
    const char * tokensFile = crgv[2];
    Text        text;
    FileSaver   saver(text, filename);
    Mouse       mouse(text);
    Keyboard    keyboard([&](void) {
        const int ASCII_BEGIN   = 32;
        const int ASCII_END     = 126;

        if ((ASCII_BEGIN <= key && key <= ASCII_END) || key == KEY_ENT)
        {
            text.insAfterCursor(string(1, (char)key));
            mouse.setIsFocused (false);
        }
    });

    Buffer      buffer;
    pair<int, int> cursor   = {0, 0};
    bool        isNeedRender= true;
    const tokens_t tokens   = readTokens(tokensFile);

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    mouseinterval(0);
    assert(has_mouse());

    Colorizer   colorizer({Colorizer::BLUE, Colorizer::BLACK});

    /**
     * initialize keyboard handlers
     */

    keyboard.addEventHandler(KEY_DOWN, [&](void)
    {
        text.moveCursor(Text::EMoveCursor::DOWN);
        mouse.setIsFocused(false);
    });

    keyboard.addEventHandler(KEY_UP, [&](void)
    {
        text.moveCursor(Text::EMoveCursor::UP);
        mouse.setIsFocused(false);
    });

    keyboard.addEventHandler(KEY_LEFT, [&](void)
    {
        text.moveCursor(Text::EMoveCursor::LEFT);
        mouse.setIsFocused(false);
    });

    keyboard.addEventHandler(KEY_RIGHT, [&](void)
    {
        text.moveCursor(Text::EMoveCursor::RIGHT);
        mouse.setIsFocused(false);
    });

    keyboard.addEventHandler(KEY_ENTER, [&](void)
    {
        text.insAfterCursor(string(1, '\n'));
        mouse.setIsFocused(false);
    });

    keyboard.addEventHandler(KEY_BACKSPACE, [&](void)
    {
        if (mouse.isFocused() && mouse.isClicked() == false)
        {
            const auto [first, second] = mouse.getFixedPos();
            buffer.set(move(*text.copyRange(first, second)));
            text.delRange(first, second);
        }
        else
        {
            text.delBeforeCursor();
        }
        mouse.setIsFocused(false);
    });

    keyboard.addEventHandler(KEY_CTRL_C, [&](void)
    {
        if (mouse.isFocused() && mouse.isClicked() == false)
        {
            auto [first, second] = mouse.getFixedPos();
            buffer.set(text.getText(), first, second);
            mouse.setIsFocused(false);
        }
    });

    keyboard.addEventHandler(KEY_CTRL_V, [&](void)
    {
        if (!buffer.isEmpty())
        {
            text.insAfterCursor(buffer.get());
            mouse.setIsFocused(false);
        }
    });

    keyboard.addEventHandler(KEY_CTRL_S, [&](void)
    {
        saver.save();
        isNeedRender = false;
    });

    keyboard.addEventHandler(KEY_CTRL_B, [&](void)
    {
        text.setCursorStartLine();
        mouse.setIsFocused(false);
    });

    keyboard.addEventHandler(KEY_CTRL_E, [&](void)
    {
        text.setCursorEndLine();
        mouse.setIsFocused(false);
    });

    keyboard.addEventHandler(KEY_CTRL_ARR_LEFT, [&](void)
    {
        text.setCursorStartFile();
        mouse.setIsFocused(false);
    });

    keyboard.addEventHandler(KEY_CTRL_ARR_RIGHT, [&](void)
    {
        text.setCursorEndFile();
        mouse.setIsFocused(false);
    });

    keyboard.addEventHandler(KEY_MOUSE, [&](void)
    {
        MEVENT event;
        if (getmouse(&event) == OK)
        {
            if (event.bstate & BUTTON1_PRESSED)
                mouse.pressEvent(screenToText);
            else if (event.bstate & BUTTON1_RELEASED)
                mouse.releaseEvent(screenToText);
            else if (event.bstate & REPORT_MOUSE_POSITION)
            {
                if (mouse.isClicked() && mouse.isFocused())
                    isNeedRender = true;
                else
                    isNeedRender = false;

                mouse.moveEvent(screenToText, event.x, event.y);
            }
        }
    });


    auto renderText = [&](void)
    {
        //clear();
        cursor = calculateText(text, mouse, colorizer, tokens);

        if (mouse.isClicked() && mouse.isFocused())
        {
            const auto [mx, my] = mouse.getPos();

            if (my == 0 && text.getOffset() > 0)
                text.decOffset();
            if ((my == getmaxy(stdscr) - 1) &&
                (text.getCursorLine() < text.getCntLines() - 1))
                text.incOffset();
        }

        while (cursor.first == -1)
        {
            if (text.getOffset() < text.getCursorLine())
                text.incOffset();
            else
                text.decOffset();

            cursor = calculateText(text, mouse, colorizer, tokens);
        }

        move(cursor.first, cursor.second);
        refresh();
    };


    /**
     * main loop
     */

    do
    {
        if (isNeedRender)
            renderText();

        if ((key = getch()) != KEY_ESC)
        {
            isNeedRender = true;
            keyboard.callEventHandler(key);
        }

    } while (key != KEY_ESC);

    endwin();
    return 0;
}

size_t screenToText(Text const& text, const pair<int,int> mousePos)
{
    int COLS, ROWS; getmaxyx(stdscr, ROWS, COLS);
    const auto [mx, my] = mousePos;
    int row             = 0;
    const size_t offset = text.getOffset();
    const size_t lineSize = to_string(offset + ROWS).size() + 2;

    const auto textToScreen = [COLS](const int row, const size_t left)
    {
        return make_pair(row + left / COLS, left % COLS);
    };

    size_t max_line = text.getOffset();

    for (size_t line = text.getOffset(); line < text.getCntLines() && row < ROWS; line++)
    {
        if (my == row && mx <= (int)lineSize)
            return text.getPos(line, 0);

        int last_row = row;

        for (size_t left = 0; left <= text.getLineSize(line) && last_row < ROWS; left++)
        {
            const auto [y, x] = textToScreen(row, left + lineSize);

            if (my == (int)y && mx == (int)x)
                return text.getPos(line, left);

            last_row = y;
        }

        if (my == last_row)
            return text.getPos(line, text.getLineSize(line));

        row = last_row + 1;
        max_line = max(max_line, line);
    }

    return text.getPos(max_line, text.getLineSize(max_line));
}

pair<int,int> calculateText(Text const& text, Mouse const& mouse, Colorizer& colorizer, tokens_t const& tokens)
{
    int COLS, ROWS;     getmaxyx(stdscr, ROWS, COLS);
    int row             = 0;
    auto cursor         = make_pair(-1,-1);
    const size_t offset = text.getOffset();
    auto textTokens     = text.getTokens();
    auto [first, second]= mouse.getFixedPos();
    const size_t maxLineSize
                        = to_string(offset + ROWS).size();

    if (second < first)
        swap(second, first);

    const auto textToScreen = [COLS](const int row, const size_t left)
    {
        return make_pair(row + left / COLS, left % COLS);
    };

    stringstream sstream;
    for (size_t line = offset; line < text.getCntLines() && row < ROWS; line++)
    {
        sstream.str("");
        string lineStr = to_string(line + 1);
        sstream << lineStr << move(string(maxLineSize - lineStr.size(), ' ')) << "] ";
        lineStr = move(sstream.str());

        if (text.getCursorLine() == line)
            cursor = textToScreen(row, text.getCursorLeft() + lineStr.size());

        colorizer.offReverse();
        colorizer.setColor(Colorizer::WHITE, Colorizer::BLACK);

        move(row, 0);
        printw(lineStr.c_str());

        int last_row    = row;
        size_t textToken= 0;

        for (size_t left = 0; left <= text.getLineSize(line) && last_row < ROWS; left++)
        {
            const auto [y, x] = textToScreen(row, left + lineStr.size());
            const size_t pos  = text.getPos (line, left);

            colorizer.setDefault();

            if (mouse.isFocused())
            {
                if (first <= pos && pos <= second)
                    colorizer.setReverse();
                else
                    colorizer.offReverse();
            }

            while (textToken < textTokens.size() &&
                   pos > textTokens[textToken].offset + textTokens[textToken].token.size())
                textToken++;

            if (colorizer.isSupport() && textToken < textTokens.size())
            {
                const size_t begin= textTokens[textToken].offset;
                const size_t end  = begin + textTokens[textToken].token.size() - 1;
                const size_t hashText = text.getTokens()[textToken].hash;

                if (begin <= pos && pos <= end && tokens.find(hashText) != tokens.end())
                    colorizer.setColor(Colorizer::YELLOW, Colorizer::BLACK);
            }

            move(y,x);
            addch(text.getText()[pos]);

            last_row = y;
        }

        row = last_row + 1;
    }

    while (row < ROWS)
    {
        move(row++, 0);
        printw("\n");
    }

    colorizer.offReverse();
    return cursor;
}

tokens_t readTokens(string const& filename)
{
    fstream file(filename, ios::in);
    tokens_t tokens;
    string token;

    while (file)
    {
        getline(file, token);
        if (file.eof()) break;

        tokens.insert({hash<string>{}(token), token});
    }

    return tokens;
}


