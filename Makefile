CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
LDFLAGS=-pthread
SRC=src#nombres directorios
OBJ=obj
BIN=bin
EMPTY=
INC=$(SRC)/headers
LIB=$(OBJ)/include
PROG=$(SRC)/cPrograms
SOURCES=$(wildcard $(SRC)/*.c)#todos los .c
LIBRARIES=$(patsubst $(INC)/%.h, $(LIB)/%.o, $(INCLUDES)) #que ponemos como biblioteca
INCLUDES=$(wildcard $(INC)/*.h)#todos los .h
PROGRAMS=$(subst $(PROG)/, $(EMPTY), $(patsubst %.c, %, $(wildcard $(PROG)/*.c)))
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
	cp $(OBJ)/$(@F) $(LIB)

.PHONY: clean
clean:
	rm -rf $(OBJ) *~ $(BIN) disco* ext*
	
