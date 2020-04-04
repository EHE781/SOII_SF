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
    int diferentes_inodos;
    if(argv[1] == NULL || argv[2] == NULL || argv[2] == NULL || argv[3] == NULL){
        fprintf(stderr,"La sintaxi correcta es: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n");
        return EXIT_FAILURE;
    }
    dir = argv[1];
    bmount(dir);
    bread(posSB, &SB);
    ninodo = reservar_inodo('f', 6);
    printf("El ninodo es: %i\n", ninodo);
    length = strlen(argv[2]);
    char *buf = malloc(length);
    strcpy(buf, argv[2]);
    int nBytes = mi_write_f(ninodo, buf, 9000, length);
    memset(buf, 0, length);
    int nbytes = mi_read_f(ninodo, buf, 9000, length);
    printf("EL texto es: ");
    for(int i = 0; i<length; i++){
            printf("%c", buf[i]);
    }
    printf("\n");
    bumount(dir);
    return EXIT_SUCCESS;
}