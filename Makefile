CC := gcc

INCLUDES := -I ./include
LIBS := -L ./lib
LDFLAGS := -lmingw32 -lSDL2main -lSDL2 -lm
CFLAGS := -O3

SRC := ./src
OBJ := ./build

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

.PHONY: all

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o ./bin/main $^ $(LIBS) $(LDFLAGS) 

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


.PHONY: clean
clean:
	del build\*.o
	del bin\main.exe