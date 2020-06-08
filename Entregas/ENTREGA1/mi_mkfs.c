//Autores: Emanuel Hegedus, Bartomeu Capo Salas, Pau Capellá Ballester
#include "bloques.h"
#include "ficheros_basico.h"
const char *cwd;
int nbloques = 0;
unsigned int tam_bloque = BLOCKSIZE;
unsigned char *buf[BLOCKSIZE];
void *bufferSB[BLOCKSIZE];
struct superbloque SB;
int main(int argc, char **argv){
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