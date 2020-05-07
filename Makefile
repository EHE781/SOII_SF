CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
#LDFLAGS=-pthread

SOURCES=mi_mkfs.c bloques.c escribir.c ficheros_basico.c ficheros.c leer.c leer_sf.c permitir.c truncar.c directorios.c pruebas.c pruebasDirectorios.c mi_mkdir.c#todos los .c
LIBRARIES=bloques.o ficheros_basico.o ficheros.o directorios.o#todos los .o de la biblioteca del SF
INCLUDES=bloques.h ficheros_basico.h ficheros.h directorios.h#todos los .h
PROGRAMS=mi_mkfs leer_sf escribir leer permitir truncar pruebas pruebasDirectorios mi_mkdir
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)