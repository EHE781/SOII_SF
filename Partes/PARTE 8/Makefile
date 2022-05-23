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
PROGRAMS=mi_mkfs leer_sf escribir leer permitir truncar pruebas pruebasDirectorios mi_mkdir mi_touch mi_ls mi_chmod mi_stat
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	@mkdir -p $(BIN)
	$(CC) $(LDFLAGS) $(LIB)/*.o $(OBJ)/$@.o -o $(BIN)/$@

$(OBJ)/%.o: $(SRC)/%.c $(INC)/*.h
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

$(LIBRARIES):
	@mkdir -p $(LIB)
	cp $(OBJ)/$@ $(LIB)

.PHONY: clean
clean:
	rm -rf $(OBJ) *~ $(BIN)
