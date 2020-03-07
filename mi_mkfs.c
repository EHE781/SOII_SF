#include "bloques.h"
#include "ficheros_basico.h"
#include "ficheros_basico.h"
const char *cwd;
int nbloques = 0;
unsigned char *buf[BLOCKSIZE];

int main(int argc, char **argv){
    memset(buf, 0, BLOCKSIZE);
    cwd = argv[1];
    bmount(cwd);
    nbloques = atoi(argv[2]);
    for(int i = 0 ; i < nbloques ; i++){
        bwrite(i, buf);
    }
    initSB(nbloques, nbloques/4); //para cada bloque un bloque y para cada bloque un inodo
    initMB();
    initAI();
    bumount();
    return 0;
}