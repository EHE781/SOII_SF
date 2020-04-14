#include "ficheros.h"

int main(int argc, char **argv){
    const char *dir;
    int nbytes = 1500;
    const void *buf_texto[nbytes];
    int offset = 0;;
    int ninodo = atoi(argv[2]);
    int total = 0;
    int check = 1;
    struct inodo inodo; 
    dir = argv[1];
    bool EndOfFile = false;
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
            write(1, buf_texto, check); //los que hemos leído esta vez
            offset += check; //los que hemos leído esta vez
            if(check == 0){
                EndOfFile = true;
            }
        }
        if(inodo.tipo == 'l'){
            fprintf(stderr, "\n¡El inodo es libre!\n");
        }
        else{
            fprintf(stderr, "\nLos bytes lógicos del inodo son: %i, y el total de bytes leídos: %i\n", inodo.tamEnBytesLog, total);
        }
        bumount(dir);
    }
}