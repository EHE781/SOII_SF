//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
#include "headers/directorios.h"

int main(int argc, char **argv){
    char *final = malloc(10);
    char *camino = malloc(10);
    camino = argv[1];
    char *inicial = malloc(10);
    char tipo;
    int i = extraer_camino(camino, inicial, final, &tipo);
    fprintf(stderr, "Le hemos pasado un %c, y el return es %i\n", tipo, i);
}