CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
#LDFLAGS=-pthread
SRC=src
OBJ=obj
BIN=bin
INC=$(SRC)/headers
LIB=$(OBJ)/include
SOURCES=$(wildcard $(SRC)/*.c)#todos los .c
LIBRARIES=bloques.o ficheros_basico.o ficheros.o directorios.o#que ponemos como biblioteca
INCLUDES=$(wildcard $(INC)/*.h)#todos los .h
PROGRAMS=mi_mkfs leer_sf escribir leer permitir truncar pruebas pruebasDirectorios
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIB)/*.o $(OBJ)/$@.o -o $(BIN)/$@

$(OBJ)/%.o: $(SRC)/%.c $(INC)/*.h
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

$(LIBRARIES):
	cp $(OBJ)/$@ $(LIB)

.PHONY: clean
clean:
	rm -rf $(wildcard $(OBJ)/*.o) $(wildcard $(LIB)/*.o) *~ $(BIN)/*
