INCLUDES= -I ./include
LIBS = -L ./lib
LDFLAGS = -lmingw32 -lSDL2main -lSDL2
FLAGS= -g

all:
	gcc ${FLAGS} ${INCLUDES} ./src/main.c ${LIBS} ${LDFLAGS} -o ./bin/main