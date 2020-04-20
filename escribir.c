#include "ficheros.h"
/*
1.Pasarlo como argumento escribiéndolo en consola y utilizar la función strlen() para calcular su longitud
2.Pasarlo como argumento haciendo el cat de cualquier fichero, por ejemplo un fichero.c de vuestra práctica de la siguiente manera:
  “$(cat dir_practica/fichero.c)”
3.Asignarlo a un buffer desde código de esta manera:
  char buffer[tamanyo];
  strcpy (buffer, "blablabla...");
4.Pasar como argumento el nombre de un fichero externo que contenga el texto
*/
int main(int argc, char **argv){
    struct superbloque SB;
    int ninodo;
    const char *dir;
    unsigned int length;
    length = strlen(argv[2]);
    char *buf = malloc(length);
    int diferentes_inodos, inodos, nbytes, nBytes = 0;
    int offset[5] = {9000, 209000, 30725000, 409605000, 480000000};
    struct STAT p_stat;
    struct tm * info;
    char afecha[24], cfecha[24], mfecha[24];
    if(argv[1] == NULL || argv[2] == NULL || argv[2] == NULL || argv[3] == NULL){
        fprintf(stderr,"La sintaxi correcta es: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n");
        return EXIT_FAILURE;
    }
    dir = argv[1];
    inodos = atoi(argv[3]);
    bmount(dir);
    bread(posSB, &SB);
    if(inodos == 0){ // si la opción de diferentes inodos es 0
      ninodo = reservar_inodo('f', 6); //reservamos inodo r/w
      strcpy(buf, argv[2]); //copiamos el texto pasado por parámetro a un buffer
      for(int i = 0; i < 5; i++){ //para cada offset el mismo inodo
        fprintf(stderr, "El nº de inodo reservado es: %i\n", ninodo);
        fprintf(stderr, "Offset: %i\n", offset[i]);
        nBytes += mi_write_f(ninodo, buf, offset[i], length);//sumamos en una variable los bytes escritos
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
      }
    }
    else if(inodos == 1){
      strcpy(buf, argv[2]);
      for(int i = 0; i < 4; i++){
        ninodo = reservar_inodo('f', 6);
        fprintf(stderr, "El nº de inodo reservado es: %i\n", ninodo);
        fprintf(stderr, "Offset: %i\n", offset[i]);
        nBytes += mi_write_f(ninodo, buf, offset[i], length);
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
      }
    }
    else{
      fprintf(stderr, "No es una opción de inodo válida, %i", inodos);
    }
    bumount(dir);
    return EXIT_SUCCESS;
}