//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
#include "../headers/directorios.h"

int main(int argc, char **argv){
    if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL){
        fprintf(stderr, "La sintaxis correcta es ./mi_chmod <disco> <permisos, 0-7> </ruta>\n");
    }else{
        if((atoi(argv[2])) < 0 || (atoi(argv[2])) > 7){
            fprintf(stderr, "Error, permisos no aceptados\n");
            return EXIT_FAILURE;
        }
        bmount(argv[1]);
        mi_chmod(argv[3], atoi(argv[2]));
        bumount(argv[1]);
    }
    return EXIT_SUCCESS;
}