#include "headers/directorios.h"

int main(int argc, char **argv){
    if(argv[1] == NULL || argv[2] == NULL){
        fprintf(stderr, "La sintaxis correcta es ./mi_stat <disco> </ruta>");
    }else{
        struct STAT p_stat;
        struct tm *tm;
        char tmp[100];
        bmount(argv[1]);
        int ninodo = mi_stat(argv[2], &p_stat);
        fprintf(stderr, " Ruta: %s\tInodo: %i\n Tamaño: %i\tBloques: %i\tTipo: %c\nEnlaces: %i\tPermisos: %i\n", argv[2],
        ninodo, p_stat.tamEnBytesLog, p_stat.numBloquesOcupados, p_stat.tipo, p_stat.nlinks, p_stat.permisos);
        tm = localtime(&p_stat.atime);
        sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d\t", tm->tm_year + 1900,
                tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        fprintf(stderr, "Acceso: %s\n", tmp);
        tm = localtime(&p_stat.mtime);
        sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d\t", tm->tm_year + 1900,
                tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        fprintf(stderr, "Modificación: %s\n", tmp);
        tm = localtime(&p_stat.ctime);
        sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d\t", tm->tm_year + 1900,
                tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        fprintf(stderr, "Cambio: %s\n", tmp);
        bumount(argv[1]);
    }
}