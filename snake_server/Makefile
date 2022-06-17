CC = gcc
CFLAG = -pthread -fsanitize=address,undefined
OBJ = falloc.o frame.o snake.o multiplayer.o

falloc: falloc.c
	$(CC) $(CFLAG) $^ -c

frame: frame.c
	$(CC) $(CFLAG) $^ -c

snake: snake.c
	$(CC) $(CFLAG) $^ -c

multiplayer: multiplayer.c
	$(CC) $(CFLAG) $^ -c

server: server.c
	$(CC) $(CFLAG) $(OBJ) $^ -o $@

all:
	make falloc frame snake multiplayer server

clean:
	rm *.o server