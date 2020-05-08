#include "ficheros.h"

int main(int argc, char **argv){
    const char *dir;
    int ninodo = atoi(argv[2]);
    struct inodo inodo; 
    dir = argv[1];
    int nbytes = 1500;
    const void *buf_texto[nbytes];
    int offset = 0;;
    int total = 0;
    int check = 1;
    bool EndOfFile = false;
    struct tm * info;
    struct STAT p_stat;
    char afecha[24], cfecha[24], mfecha[24];
    if(argv[1] == NULL || argv[2] == NULL){
        fprintf(stderr, "La sintaxi correcta es <camino a SF> <ninodo>\n");
    }
    else{
        bmount(dir);
        leer_inodo(ninodo, &inodo);
        while(inodo.tipo != 'l' && !EndOfFile){
            memset(buf_texto, 0, nbytes);
            total += mi_read_f(ninodo, buf_texto, offset, nbytes);
            check = mi_read_f(ninodo, buf_texto, offset, nbytes);
            if(check == EXIT_FAILURE){
                bumount(dir);
                return EXIT_FAILURE;
            }
            if(check < nbytes){
                const char *buf_aux[check];
                memcpy(buf_aux, buf_texto, check);
                write(1, buf_aux, check);
            }else{
                write(1, buf_texto, check); //los que hemos leído esta vez
            }
            if(check == 0){
                EndOfFile = true;
            }
            offset += check; //los que hemos leído esta vez
        }
        if(inodo.tipo == 'l'){
            fprintf(stderr, "\n¡El inodo es libre!\n");
        }
        else{
            mi_stat_f(ninodo, &p_stat);
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
            fprintf(stderr, "\nLos bytes lógicos del inodo son: %i, y el total de bytes leídos: %i\n", 
            inodo.tamEnBytesLog, total);
        }
        bumount(dir);
    }
}