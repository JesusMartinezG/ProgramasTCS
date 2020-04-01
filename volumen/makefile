PROYECTO=volumen
CFLAGS=-Wall
LDFLAGS=-lm
CC=gcc
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

all: $(PROYECTO)

%.o: %.c
					$(CC) -c $< $(CFLAGS)

$(PROYECTO): $(OBJS)
					$(CC) $^ -o $@	$(LDFLAGS)

.PHONY: clean

clean:
					rm -f *.o $(PROYECTO)