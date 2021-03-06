#include "../headers/directorios.h"

int main (int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "La sintaxis correcta es ./mi_rm disco /ruta\n");
        return EXIT_FAILURE;
    }
    char *aux = argv[2];
    if(aux[strlen(aux) - 1] == '/'){
        fprintf(stderr, "No es un fichero válido\n");
        return EXIT_FAILURE;
    }
    bmount(argv[1]);
    mi_unlink(argv[2], false);
    bumount(argv[1]);
    return EXIT_SUCCESS;
}