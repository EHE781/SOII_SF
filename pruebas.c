#include "directorios.h"

int main(int argc, char **argv){
    char *final = malloc(10);
    char *camino = malloc(10);
    camino = "/directorio/fichero.txt";
    char *inicial = malloc(10);
    char *tipo = malloc(1);
    tipo = 'a';
    int i = extraer_camino(camino, inicial, final, tipo);
    fprintf(stderr, "Le hemos pasado un %c, y el return es %i\n", tipo, i);
}