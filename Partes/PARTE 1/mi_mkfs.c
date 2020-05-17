#include "bloques.h"
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
    bumount();
    return 0;
}