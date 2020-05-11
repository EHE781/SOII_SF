#include "headers/directorios.h"

int main(int argc, char **argv)
{
    if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL || argv[4] == NULL)
    {
        fprintf(stderr, "La sintaxis correcta es ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return EXIT_FAILURE;
    }
    if ((argv[2][strlen(argv[2]) - 1]) == '/'){
        fprintf(stderr, "No es un fichero\n");
        return EXIT_FAILURE;
    }
    int bytesEscritos;
    bmount(argv[1]);
    bytesEscritos = mi_write(argv[2], argv[3], atoi(argv[4]), strlen(argv[3]));
    if(bytesEscritos == -1){ //si no ha escrito, decimos que ha escrito 0 bytes(no -1)
        bytesEscritos += 1;
    }
    bumount(argv[1]);
    fprintf(stderr, "Se han escrito %d bytes\n", bytesEscritos);
    return EXIT_SUCCESS;
}