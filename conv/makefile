PROYECTO=convolucion
CFLAGS= -g3
LDFLAGS=-lm
CC=gcc
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

all: $(PROYECTO)

%.o: %.c
					$(CC) -c $< $(CFLAGS)

$(PROYECTO): $(OBJS)
					$(CC) $^ -o $@ $(LDFLAGS) -g3

.PHONY: clean

clean:
					rm -f *.o $(PROYECTO)