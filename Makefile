CC=g++

CFLAGS_RELEASE=-g0 -O3
CFLAGS_DEBUG=-ggdb -g3 -O1 -pg
CFLAGS=-Wall -Wextra -Wswitch-enum -pedantic -std=gnu++17
LFLAGS=-lncurses

SRC=./src/main.cpp ./src/text.cpp ./src/mouse.cpp
INC=./inc/text.hpp ./inc/mouse.hpp ./inc/buffer.hpp ./inc/coloriser.hpp ./inc/keyboard.hpp ./inc/file_saver.hpp
OBJ=$(SRC:.cpp=.o)

GARBAGE=*.o *.out $(OBJ) main
TRASH=2>/dev/null
DEF_TOKEN_FILE=./src/tokens/tokens_c_and_cpp.txt

release: $(SRC) $(INC)
	$(CC) $(CFLAGS) $(CFLAGS_RELEASE) $(LFLAGS) -o main $(SRC) $(LFLAGS)
	./main index.txt $(DEF_TOKEN_FILE)

debug: $(SRC) $(INC)
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG)  $(LFLAGS) -o main $(SRC) $(LFLAGS)
	./main index.txt $(DEF_TOKEN_FILE)

clean:
	-rm $(GARBAGE) $(TRASH)
