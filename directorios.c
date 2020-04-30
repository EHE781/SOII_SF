#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    bool hayDir = false;
    if(camino[0] == '/'){
        for (int i = 1; i < strlen(camino); i++){   
            if(camino[i] == '/'){   //caso en el que le pasemos //algo.txt no gestionado(doble barra)
                strncpy(inicial, camino, sizeof(char) * i - 1);
                hayDir = true;
                strcpy(final, camino + i);
                *tipo = 'd';
            }
        }
        if(!hayDir){
            strncpy(inicial, camino + 1, sizeof(char) * strlen(camino) - 1);//+1 para evitar la primera '/'
            *tipo = 'f';
        }
        return 0;
    }
    else{
        fprintf(stderr, "No existe el camino o directorio\n");
        return EXIT_FAILURE;
    }
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir,
unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
    int error = 0;
    struct superbloque SB;
    bread(posSB, &SB);
    if(camino_parcial[0] == '/' && strlen(camino_parcial) == 1){
        *p_inodo = SB.posInodoRaiz; //inodo raíz
        *p_entrada = 0; //entrada 0 pertenece a inodo raíz
        return EXIT_SUCCESS;
    }
    struct entrada entrada;
    char inicial[sizeof(entrada.nombre)];
    char final[sizeof(strlen(camino_parcial))];
    char tipo;
    struct inodo inodo_dir;
    error = extraer_camino(camino_parcial, inicial, final, &tipo);
    if(error == EXIT_FAILURE){
        return ERROR_CAMINO_INCORRECTO;
    }
    error = leer_inodo(*p_inodo_dir, &inodo_dir);
    if(error == EXIT_FAILURE){
        return ERROR_PERMISO_LECTURA;
    }
    unsigned char *buf_entradas[BLOCKSIZE / sizeof(struct entrada)] = malloc(BLOCKSIZE);
    int num_entradas = 0;
    if(inodo_dir.tamEnBytesLog == 0){
        //ERROR NO HAY NADA EN EL INODO
    }
    else{
        num_entradas = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
    }
}