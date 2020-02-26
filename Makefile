CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
<<<<<<< HEAD
LDFLAGS=#-pthread

SOURCES=ficheros_basico.c bloques.c mi_mkfs.c #todos los .c
LIBRARIES=ficheros_basico.o bloques.o #todos los .o de la biblioteca del SF
INCLUDES=ficheros_basico.h bloques.h #todos los .h
=======
#LDFLAGS=-pthread

SOURCES=bloques.c mi_mkfs.c #todos los .c
LIBRARIES=bloques.o #todos los .o de la biblioteca del SF
INCLUDES=bloques.h #todos los .h
>>>>>>> 93f611ccc4fa8532ea6816c1bc67985a015665fe
PROGRAMS=mi_mkfs 
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

<<<<<<< HEAD
	$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)
=======
$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
    $(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES)
    $(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
    rm -rf *.o *~ $(PROGRAMS)
>>>>>>> 93f611ccc4fa8532ea6816c1bc67985a015665fe
