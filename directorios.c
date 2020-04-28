#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    bool hayDir = false;
    if(camino[0] == '/'){
        for (int i = 1; i < strlen(camino); i++){   
            if(camino[i] == '/'){   //caso en el que le pasemos //algo.txt no gestionado(doble barra)
                strncpy(inicial, camino, sizeof(char) * i - 1);
                hayDir = true;
                strcpy(final, camino + i);
                tipo = 'd';
                return 1;
            }
        }
        if(!hayDir){
            strncpy(inicial, camino + 1, sizeof(char) * strlen(camino) - 1);//+1 para evitar la primera '/'
            tipo = 'f';
            return 0;
        }
    }
    else{
        return EXIT_FAILURE;
    }
}