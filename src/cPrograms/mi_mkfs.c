#include "../headers/bloques.h"
#include "../headers/ficheros_basico.h"
const char *cwd;
int nbloques = 0;
unsigned int tam_bloque = BLOCKSIZE;
unsigned char *buf[BLOCKSIZE];
void *bufferSB[BLOCKSIZE];
struct superbloque SB;
int main(int argc, char **argv){
    if(argv[1] == NULL || argv[2] == NULL){
        fprintf(stderr, "La sintaxis correcta es ./mi_mkfs <nombre_disco> <tamaño>");
    }
    memset(buf, 0, tam_bloque * sizeof(char));
    cwd = argv[1];
    bmount(cwd);
    nbloques = atoi(argv[2]);
    for(int i = 0 ; i < nbloques ; i++){
        bwrite(i, buf);
    }
    initSB(nbloques, nbloques/4); //para cada bloque un bloque y para cada bloque un inodo
    initMB();
    initAI();
    reservar_inodo('d', 7); //creamos el directorio raíz
    bumount();
    return 0;
}