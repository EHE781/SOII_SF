#include "directorios.h"

int main(int argc, char **argv){
    if(argv[1] == NULL || argv[2] == NULL || argv[3] == NULL){
        fprintf(stderr, "La sintaxis correcta es \" ./mkdir <disco> <permisos> <ruta>\"\n");
    }else{
        int length = strlen(argv[3]);
        if(argv[3][length - 1] != '/'){ //si es un fichero
            fprintf(stderr, "No es una ruta de directorio válida\n");
        }else{
            int permisos = atoi(argv[2]);
            if(permisos < 0 || permisos > 7){
                fprintf(stderr, "Los permisos no son válidos\n");
            }else{
                int error = mi_creat(argv[3], permisos);
                if(error == EXIT_FAILURE){
                    return EXIT_FAILURE;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}