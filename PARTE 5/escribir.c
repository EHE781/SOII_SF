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
    void *buf;
    if(argv[1] == NULL){
        fprintf(stderr,"La sintaxi correcta es \'-t \"texto\"\' o bien \'directorio\'\n");
        return EXIT_FAILURE;
    }
    if(argv[1] == "-t"){
        if(argv[2] == NULL){
            fprintf(stderr,"La sintaxi correcta es \'-t \"texto\"\' o bien \'directorio\'\n");
            return EXIT_FAILURE;
        }
        buf = argv[2];
    }else{
        buf = argv[1];
        printf("jj");
    }
    return EXIT_SUCCESS;
}