#include "ficheros.h"

int main(int argc,char **argv){
    const char *dir;
    int ninodo = atoi(argv[2]);
    int permisos = atoi(argv[3]); 
    dir = argv[1];
    if(argv[1] == NULL || argv[2] == NULL || argv[3] == NULL){
        fprintf(stderr, "\nSintaxis: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
        return EXIT_FAILURE;
    }
    else{
        bmount(dir);
        mi_chmod_f(ninodo, permisos);
        bumount(dir);
    }

    return EXIT_SUCCESS;
}