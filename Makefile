CC=g++
CFLAGS=-Wall -Wextra -Wswitch-enum -pedantic -ggdb -pg -std=gnu++17
LFLAGS=-lncurses
SRC=./src/main.cpp ./src/text.cpp ./src/mouse.cpp
INC=./inc/text.hpp ./inc/mouse.hpp ./inc/buffer.hpp ./inc/coloriser.hpp ./inc/keyboard.hpp ./inc/file_saver.hpp
OBJ=$(SRC:.cpp=.o)

GARBAGE=*.o *.out $(OBJ) main
TRASH=2>/dev/null
DEF_TOKEN_FILE=./src/tokens/tokens_c_and_cpp.txt

target: $(SRC) $(INC)
	$(CC) $(CFLAGS) $(LFLAGS) -o main $(SRC) $(LFLAGS)
	./main index.txt $(DEF_TOKEN_FILE)

clean:
	-rm $(GARBAGE) $(TRASH)
