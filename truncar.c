#include "ficheros.h"

int main(int argc, char **argv){
    const char *dir;
    int ninodo, nbytes;
    if(argv[1] == NULL || argv[2] == NULL || argv[3] == NULL){
        fprintf(stderr, "\nSintaxis: truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
        return EXIT_FAILURE;
    }
    else{
        dir = argv[1];
        ninodo = atoi(argv[2]);
        nbytes = atoi(argv[3]);
        bmount(dir);
        if (nbytes == 0){
            liberar_inodo(ninodo);
        }
        else{
            mi_truncar_f(ninodo, nbytes);
        }
        bumount(dir);
    }
    return EXIT_SUCCESS;
}