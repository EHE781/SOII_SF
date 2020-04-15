#include "ficheros.h"

int main(int argc, char **argv){
    const char *dir;
    int ninodo, nbytes;
    struct STAT p_stat;
    dir = argv[1];
    ninodo = atoi(argv[2]);
    nbytes = atoi(argv[3]);
    if(argv[1] == NULL || argv[2] == NULL || argv[3] == NULL){
        fprintf(stderr, "\nSintaxis: truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
        return EXIT_FAILURE;
    }
    else{
        bmount(dir);
        if (nbytes == 0){
            liberar_inodo(ninodo);
        }
        else{
            mi_truncar_f(ninodo, nbytes);
        }
        mi_stat_f(ninodo, &p_stat);
        bumount(dir);
        printf("El tamaño en bytes lógicos del inodo es: %i, los bloques ocupados: %i\n",
        p_stat.tamEnBytesLog, p_stat.numBloquesOcupados);
    }
    return EXIT_SUCCESS;
}