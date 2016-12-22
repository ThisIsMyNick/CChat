CC = gcc
CFLAGS = -Wall
TARGET = cchat
LIBS = -lncurses

SRCS = src/client/main.c src/client/server.c src/client/client.c src/client/window.c src/client/message.c

OBJS = $(SRCS:.c=.o)

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LIBS) $(EXTRA)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ $(EXTRA)

clean:
	rm -f src/client/*.o
	rm -f cchat
	rm -f window
