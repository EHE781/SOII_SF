#include "ficheros.h"

int main(int argc, char **argv){
    if(argv[1] == NULL || argv[2] == NULL || argv[3] == NULL){
        fprintf(stderr, "\nSintaxis: truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
        return EXIT_FAILURE;
    }
    return 0;
}