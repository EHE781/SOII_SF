#include "ficheros.h"
#include <string.h>

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);
struct entrada {
char nombre[60];  //En el SF ext2 la longitud del nombre es 256
unsigned int ninodo;
};
