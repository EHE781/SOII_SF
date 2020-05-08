#include "headers/ficheros.h"


int main(int argc, char **argv){
    const char *dir;
    int ninodo, nbytes;
    struct tm * info;
    struct STAT p_stat;
    char afecha[24], cfecha[24], mfecha[24];
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
        strftime(afecha, 24, "%a %d-%m-%Y %H:%M:%S", info = localtime(&p_stat.atime));
        strftime(cfecha, 24, "%a %d-%m-%Y %H:%M:%S", info = localtime(&p_stat.ctime));
        strftime(mfecha, 24, "%a %d-%m-%Y %H:%M:%S", info = localtime(&p_stat.mtime));
        fprintf(stderr, "DATOS INODO %i\n\
        tipo=%c\n\
        permisos=%i\n\
        atime: %s\n\
        ctime: %s\n\
        mtime: %s\n\
        nlinks: %i\n\
        tamEnBytesLog=%i\n\
        numBloquesOcupados=%i\n",
        ninodo, p_stat.tipo, p_stat.permisos, afecha, cfecha, mfecha,
        p_stat.nlinks, p_stat.tamEnBytesLog, p_stat.numBloquesOcupados);
    }
    return EXIT_SUCCESS;
}