CC = gcc
CFLAGS = -Wall
TARGET = cchat

SRCS = src/client/main.c src/client/server.c src/client/client.c

OBJS = $(SRCS:.c=.o)

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

window: src/client/window.c
	$(CC) $(CFLAGS) src/client/window.c -o window -DWINDOW_ONLY -lncurses

clean:
	rm -f src/client/*.o
	rm -f cchat
	rm -f window
