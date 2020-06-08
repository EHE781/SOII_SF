//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
#include "headers/directorios.h"

int main (int argc, char **argv){
    if(argv[1] == NULL || argv[2] == NULL){
        fprintf(stderr, "La sintaxis correcta es ./mi_cat <disco> </ruta_fichero>");
        return EXIT_FAILURE;
    }
    if((argv[2][strlen(argv[2]) - 1]) == '/'){
        fprintf(stderr, "No se puede leer un directorio\n");
        return EXIT_FAILURE;
    }
    int tamBuffer = BLOCKSIZE * 4;
    char *lectura = malloc(tamBuffer);
    int offset = 0;
    bmount(argv[1]);
    int bytesLeidos = mi_read(argv[2], lectura, offset, tamBuffer);
    fprintf(stderr, "\nSe han leído %d bytes del fichero\n", bytesLeidos);
    bumount(argv[1]);
    return EXIT_SUCCESS;
}