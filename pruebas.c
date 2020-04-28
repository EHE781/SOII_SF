#include "directorios.h"

int main(int argc, char **argv){
    char *final = malloc(10);
    char *camino = malloc(10);
    camino = "/directorio/fichero.txt";
    char *inicial = malloc(10);
    char *tipo = malloc(10);
    tipo = 'd';
    int i = extraer_camino(camino, inicial, final, tipo);
}