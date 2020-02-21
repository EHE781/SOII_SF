CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
#LDFLAGS=-pthread

SOURCES=bloques.c mi_mkfs.c #todos los .c
LIBRARIES=bloques.o #todos los .o de la biblioteca del SF
INCLUDES=bloques.h #todos los .h
PROGRAMS=mi_mkfs 
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
    $(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES)
    $(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
    rm -rf *.o *~ $(PROGRAMS)