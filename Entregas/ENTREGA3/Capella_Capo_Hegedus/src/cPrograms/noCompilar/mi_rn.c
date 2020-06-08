//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
#include "../headers/directorios.h"

int main(int argc, char **argv){
    if(argc != 4){
        fprintf(stderr, "La sintaxis correcta es ./mi_rn <disco> <fuente> <destino>\n");
        return EXIT_FAILURE;
    }
    bmount(argv[1]);
    mi_rn(argv[2], argv[3]);
    bumount(argv[1]);
    return EXIT_SUCCESS;
}