#include "headers/directorios.h"

int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "La sintaxis correcta es ./mi_link    disco    /ruta_fichero_original      /ruta_enlace\n");
    }
    if (argv[2][strlen(argv[2]) - 1] != '/' && argv[3][strlen(argv[3]) - 1] != '/')
    {
    bmount(argv[1]);
    mi_link(argv[2], argv[3]);
    bumount(argv[1]);
    }else
    {
        fprintf(stderr, "No se admiten directorios\n");
        return EXIT_FAILURE;
    }
}