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
    int diferentes_inodos,nBytes,nbytes,inodos;
    int offset[4] = {9000, 209000, 30725000, 409605000};
    if(argv[1] == NULL || argv[2] == NULL || argv[2] == NULL || argv[3] == NULL){
        fprintf(stderr,"La sintaxi correcta es: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n");
        return EXIT_FAILURE;
    }
    dir = argv[1];
    inodos = atoi(argv[3]);
    bmount(dir);
    bread(posSB, &SB);
    if(inodos == 0){
    ninodo = reservar_inodo('f', 6);
    for(int i = 0; i < 4; i++){
        printf("El ninodo es: %i\n", ninodo);
        strcpy(buf, argv[2]);
        nBytes += mi_write_f(ninodo, buf, offset[i], length);
        memset(buf, 0, length);
        nbytes += mi_read_f(ninodo, buf, offset[i], length);
        write(1, buf, length);
      }
    }
    else if(inodos == 1){
      for(int i = 0; i < 4; i++){
        ninodo = reservar_inodo('f', 6);
        printf("El ninodo es: %i\n", ninodo);
        strcpy(buf, argv[2]);
        nBytes += mi_write_f(ninodo, buf, offset[i], length);
        memset(buf, 0, length);
        nbytes += mi_read_f(ninodo, buf, offset[i], length);
        write(1, buf, length);
      }
    }
    else{
      printf("No es una opción de inodo válida, %s", inodos);
    }
    bumount(dir);
    return EXIT_SUCCESS;
}