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
NOTPROG=$(PROG)/noCompilar
VPATH=$(SRC)/:$(PROG)/:$(NOTPROG)/
SOURCES=$(wildcard $(SRC)/*.c) $(wildcard $(PROG)/*.c) $(wildcard $(NOTPROG)/*.c)#todos los .c
LIBRARIES=$(patsubst $(INC)/%.h, $(LIB)/%.o, $(INCLUDES)) #que ponemos como biblioteca
INCLUDES=$(wildcard $(INC)/*.h)#todos los .h
PROGRAMS=$(subst $(PROG)/, $(EMPTY), $(patsubst %.c, %, $(wildcard $(PROG)/*.c)))
OBJS=$(patsubst %.c, $(OBJ)/%.o, $(notdir $(SOURCES)))

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	@mkdir -p $(BIN)
	$(CC) $(LDFLAGS) $(LIB)/*.o $(OBJ)/$@.o -o $(BIN)/$@

$(OBJ)/%.o: %.c $(INC)/*.h
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

$(LIBRARIES):
	@mkdir -p $(LIB)
	cp $(OBJ)/$(@F) $(LIB)

.PHONY: clean
clean:
	rm -rf $(OBJ) *~ $(BIN) disco* ext*
	
