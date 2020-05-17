#include "headers/directorios.h"

int main (int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "La sintaxis correcta es ./mi_rm disco /ruta");
        return EXIT_FAILURE;
    }
    if(argv[2][strlen(argv[2] - 1)] == '/'){
        fprintf(stderr, "No es un fichero válido\n");
        return EXIT_FAILURE;
    }
    bmount(argv[1]);
    mi_unlink(argv[2], false);
    bumount(argv[1]);
    return EXIT_SUCCESS;
}